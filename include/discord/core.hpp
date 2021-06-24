#pragma once

#include "discord/raw/gateway.hpp"
#include "discord/raw/http_interface.hpp"
#include "memory_file.hpp"
#include "memory_json.hpp"
#include "printer.hpp"
#include "tasker.hpp"

#include <vector>
#include <mutex>
#include <functional>
#include <future>
#include <memory>

//#define LSW_DEBUG_HTTP_REQUESTS

namespace LSW {
    namespace PocketDiscord {

        constexpr int max_bot_tries = 5;
        constexpr size_t default_task_oncore_size = 24 * 1024; // bytes

        struct request_response {
            MemoryFile mf;
            int code = 0;
            MemoryFileJSON mj;

            request_response();
            request_response(const request_response&) = delete;
            request_response(request_response&&);

            void operator=(const request_response&) = delete;
            void operator=(request_response&&) = delete;
        };

        class BotCore {
            EventPoll http_poll;

            // - - - - - GATEWAY STUFF - - - - - //
            GatewayControl gateway;
            std::function<void(const gateway_events&, const MemoryFileJSON&)> function; // save, gateway events
            HttpConnection g_http;

            void http_thr();
        public:
            ~BotCore();

            bool is_connected_and_ready() const;

            bool setup(const std::string&, const int, std::function<void(const gateway_events&, const MemoryFileJSON&)>);
            void stop();

            // path (discord relative path), method (HTTP method), data (if any JSON data)
            std::future<request_response> post_task(const std::string&, const http_request&, const std::string&);
            
            unsigned long long get_bot_id() const;
            const std::string& get_bot_name() const;

            GatewayControl& _get_gateway();
        };

        template<typename T, typename... Args>
        inline std::future<T> fake_future(Args... args) {
            std::packaged_task<T()> task_todo([&]{ return T(args...); });
            std::future<T> result = task_todo.get_future();
            task_todo();
            return result; // empty result
        }

        template<typename T>
        inline std::future<T> fake_future() {
            std::packaged_task<T()> task_todo([&]{ return T(); });
            std::future<T> result = task_todo.get_future();
            task_todo();
            return result; // empty result
        }

    }
}