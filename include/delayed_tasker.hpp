#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"

#include "printer.hpp"
#include "discord/raw/time_iso.hpp"

#include <functional>
#include <vector>
#include <mutex>
#include <thread>

namespace LSW {
    namespace PocketDiscord {

        constexpr size_t max_delayed_tasks_g = 30;
        
        class DelayedTasker {
            struct pair_me{
                unsigned long long first = 0;
                std::function<void(void)> second;
            };

            std::vector<pair_me> tasks;
            std::mutex tasks_mtx;
            std::thread thr;
            bool keep_running = false;

            void keep_tasking();
        public:
            DelayedTasker(const bool = false);

            void start();
            void stop();

            bool post(const unsigned long long, std::function<void(void)>); // offset in ms
        };

        inline DelayedTasker global_delayer(true);

    }
}