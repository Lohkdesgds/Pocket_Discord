/*
* # LED_CONTROL
* Verified: < 20210520
*/

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"

#include <mutex>

#include "tasker.hpp"

namespace LSW {
    namespace PocketDiscord {

        const gpio_num_t led_on_board = GPIO_NUM_2; // from ARDUINO, default LED pin for ESP32
        constexpr int32_t max_led_queue_size = 32;

        class LedControl {
            EventPoll led_poll;
            gpio_num_t led_port = led_on_board;
            std::mutex mtx;
        public:
            // Kill
            ~LedControl();

            // Setup with custom GPIO port
            bool setup(const gpio_num_t = led_on_board);
            // Stops internal thread
            void stop();

            // Add a blink task to do (how many blinks, time (each))
            void blink(const unsigned = 1, const unsigned = 100);

            void force_high();
            void force_low();
        };

    }
}