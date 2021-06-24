#include "delayed_tasker.hpp"


namespace LSW {
    namespace PocketDiscord {

        void DelayedTasker::keep_tasking()
        {
            while (keep_running) {
                vTaskDelay(pdMS_TO_TICKS(250));

                std::lock_guard<std::mutex> luck(tasks_mtx);
                const unsigned long long timeneow = get_time_now_ms();

                for(size_t pos = 0; pos < tasks.size(); pos++)
                {
                    const auto& i = tasks[pos];

                    if (i.first <= timeneow){
                        try {
                            std::function<void(void)> func = std::move(i.second);
                            tasks.erase(tasks.begin() + pos--);
                            if (func) func();
                        }
                        catch(const std::exception& e) {
                            logg << L::SL << Color::RED << "[DelayedTasker] Event task on core #" << xPortGetCoreID() << " got an exception: " << e.what() << "." << L::EL;
                        }
                        catch(...) {
                            logg << L::SL << Color::RED << "[DelayedTasker] Event task on core #" << xPortGetCoreID() << " got an exception: UNCAUGHT." << L::EL;
                        }
                    }
                }
            }
        }
        
        DelayedTasker::DelayedTasker(const bool autostart)
        {
            if (autostart) start();
        }

        void DelayedTasker::start()
        {
            if (keep_running) stop();
            keep_running = true;
            thr = std::thread([&]{keep_tasking();});
        }

        void DelayedTasker::stop()
        {
            if (keep_running){
                keep_running = false;
                thr.join();
                for(auto& i : tasks){
                    try { if (i.second) i.second(); } catch (...) {}
                }
                tasks.clear();
            }
        }

        bool DelayedTasker::post(const unsigned long long dt, std::function<void(void)> f)
        {
            if (tasks.size() >= max_delayed_tasks_g) return false; // too busy
            const unsigned long long timeneow = get_time_now_ms();
            std::lock_guard<std::mutex> luck(tasks_mtx);
            tasks.push_back({timeneow + dt, f});
            return true;
        }

    }
}