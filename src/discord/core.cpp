#include "discord/core.hpp"

namespace LSW {
    namespace PocketDiscord {

        request_response::request_response()
            : mj(mf)
        {            
        }
        
        request_response::request_response(request_response&& oth)
            : mf(std::move(oth.mf)), code(oth.code), mj(mf)
        {
        }

        BotCore::~BotCore()
        {
            stop();
        }

        bool BotCore::is_connected_and_ready() const
        {
            return gateway.is_connected_and_ready();
        }

        bool BotCore::setup(const std::string& token, const int intents, std::function<void(const gateway_events&, const MemoryFileJSON&)> func)
        {
            stop();
            function = func;

            if (!gateway.setup(token, intents, function)) return false;

            if (!g_http.setup(token, false)) return false;

            if (!http_poll.setup("HTTPQueue", queue_task_size_default, 0, default_task_oncore_size)) return false;

            return true;
        }

        void BotCore::stop()
        {
            g_http.close();
            gateway.stop();
            http_poll.reset();
        }

        std::future<request_response> BotCore::post_task(const std::string& path, const http_request& method, const std::string& data)
        {

            std::shared_ptr<std::packaged_task<request_response()>> tassss = std::make_shared<std::packaged_task<request_response()>>(
            [&, path, method, data]{
#ifdef LSW_DEBUG_HTTP_REQUESTS
                logg << L::SL << Color::BRIGHT_GRAY << "[BotCore] Running path=" << path << " method=" << static_cast<int>(method) << " json=" << data << L::EL;
#endif

                request_response pr;
                // ratelimit try again.

                for (int try_again = 0; try_again < max_bot_tries;) {

                    if (!g_http.task(path, method, data)) {
                        logg << L::SL << Color::YELLOW << "[BotCore] Post task failed to task once. (" << path << ", method=" << static_cast<int>(method) << ")" << L::EL;
                        try_again++;
                        vTaskDelay(pdMS_TO_TICKS(200));                        
                        continue;
                    }

                    if (!g_http.has_task_ended()) {
                        logg << L::SL << Color::GREEN << "[BotCore] Post task waiting task to end..." << L::EL;

                        while(!g_http.has_task_ended()) vTaskDelay(pdMS_TO_TICKS(50));
                    }

                    pr.code = g_http.response_code();

                    pr.mf = std::move(g_http.get_data());

                    if (pr.mf.empty()) return std::move(pr);

                    // https://discord.com/developers/docs/topics/rate-limits#rate-limits
                    const std::string limit_now = g_http.get_header("x-ratelimit-remaining");

                    if (pr.code == 429) // bad news, overload (how?)
                    {
                        logg << L::SL << Color::YELLOW << "[BotCore] Post task got RATE LIMITED!" << L::EL;
                        const std::string retry_after = g_http.get_header("retry-after");

                        if (pr.mj.is_empty() && retry_after.empty()) {
                            logg << L::SL << Color::YELLOW << "[BotCore] Post task can't parse JSON correctly. Waiting 15 seconds instead!" << L::EL;
                            vTaskDelay(pdMS_TO_TICKS(15000));
                        }
                        else {
                            int timee = 0;
                            
                            if (!pr.mj.is_empty()) timee = 1 + pr.mj["retry_after"].as_int();
                            else timee = atoi(retry_after.c_str());

                            if (timee > 60 || timee < 0) {
                                logg << L::SL << Color::YELLOW << "[BotCore] RATE LIMIT value was invalid! It was " << timee << " second(s). Doing 20 seconds instead." << L::EL;
                                timee = 20;
                            }
                            else logg << L::SL << Color::YELLOW << "[BotCore] Waiting " << timee << " second(s)." << L::EL;

                            vTaskDelay(pdMS_TO_TICKS(timee * 1000));
                        }
                        try_again++;
                        vTaskDelay(pdMS_TO_TICKS(200));                        
                        continue;
                    }
                    else if (!limit_now.empty()) { // no need for retry.
                        int remains = 0;
                        if (sscanf(limit_now.c_str(), "%d", &remains) == 1){
                            if (remains <= 1) {
                                logg << L::SL << Color::YELLOW << "[BotCore] Ratelimit is near!" << L::EL;

                                const std::string time_point = g_http.get_header("x-ratelimit-reset");
                                if (time_point.empty()) {
                                    logg << L::SL << Color::YELLOW << "[BotCore] Can't get point in time for rate limit! Waiting a second instead." << L::EL;
                                    vTaskDelay(pdMS_TO_TICKS(1000));
                                }
                                else {
                                    double point_time = 0.0;
                                    if (sscanf(time_point.c_str(), "%lf", &point_time) == 1) {
                                        point_time++;
                                        const double noww = (get_time_now_ms() * 1.0 / 1000);
                                        const double diff_ms = 1000.0 * (noww > point_time ? 0 : point_time - noww);

                                        logg << L::SL << Color::YELLOW << "[BotCore] Ratelimit holding for " << diff_ms << " ms." << L::EL;
                                        
                                        vTaskDelay(pdMS_TO_TICKS(diff_ms));
                                    }
                                }
                            }
                        }
                    }
                    
                    break; // good
                }
                return std::move(pr); // end
            });

            std::future<request_response> result = tassss->get_future();

            for(int errs = 0; (
            http_poll.post([tassss]{
                if (tassss && tassss->valid()) (*tassss)();
                else logg << L::SL << Color::RED << "[BotCore] HTTP request was lost somehow." << L::EL;
            }) != event_result::NO_ERROR); errs++){
                logg << L::SL << Color::RED << "[BotCore] One delayed HTTP task failed to be added! [" << (errs + 1) << "/5]" << L::EL;
                if (errs + 1 >= 5){
                    logg << L::SL << Color::RED << "[BotCore] Failed to task HTTP request." << L::EL;
                    return fake_future<request_response>();
                }
                else vTaskDelay(pdMS_TO_TICKS(2500));
            }

            return std::move(result);
        }

        unsigned long long BotCore::get_bot_id() const
        {
            return gateway.get_bot_id();
        }

        const std::string& BotCore::get_bot_name() const
        {
            return gateway.get_bot_name();
        }

        GatewayControl& BotCore::_get_gateway()
        {
            return gateway;
        }
    }
}