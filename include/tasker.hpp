/*
* # TASKER
* Verified: 20210520
*/

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"

#include "printer.hpp"

#include <functional>
#include <mutex>

ESP_EVENT_DECLARE_BASE(EVENT_LSW_HANDLER);

namespace LSW {
    namespace PocketDiscord {

        //const bool show_debug = true;

        constexpr size_t default_task_size = 6 * 1024; // bytes
        constexpr unsigned timeout_task_time = 5000; // ms
        constexpr unsigned queue_task_size_default = 5;
        constexpr unsigned default_task_priority = 4;
        
        enum event_result {
            NO_ERROR,   // Good!
            TIMEOUT,    // Timeout, took longer than expected.
            ERROR       // other errors.
        };

        struct data_pass {
            std::function<void(void)>* func = nullptr;
            unsigned* counterr = nullptr;

            void copy(std::function<void(void)>, unsigned*);
            void freeup();

            void run_if();
        };

        void __eventpoll_static_task(void*, esp_event_base_t, int32_t, void*);

        class EventPoll {
            esp_event_loop_handle_t loop_handle = nullptr;
            unsigned max_timeout = timeout_task_time;
            bool is_setup = false;
            unsigned queue_siz_inn = 0;
        public:
            ~EventPoll();

            bool setup(const std::string&, const int32_t = queue_task_size_default, const unsigned = 0, const size_t = default_task_size, const unsigned = default_task_priority);

            event_result post(const std::function<void(void)>);

            void reset();

            unsigned queue_size_now() const;
        };
    }
}