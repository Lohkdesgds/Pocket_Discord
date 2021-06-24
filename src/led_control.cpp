#include "led_control.hpp"

namespace LSW {
    namespace PocketDiscord {

        LedControl::~LedControl()
        {
            stop();
        }

        bool LedControl::setup(const gpio_num_t port)
        {
            logg << L::SL << Color::GREEN << "[LedControl] Setting up port #" << static_cast<int>(port) << "..." << L::EL;

            stop();
            
            led_port = port;

            gpio_reset_pin(led_port);
            gpio_set_direction(led_port, GPIO_MODE_OUTPUT);

            if (!led_poll.setup("LedP" + std::to_string(static_cast<int>(port)), max_led_queue_size)) {
                logg << L::SL << Color::RED << "[LedControl] Failed to setup port. Queue failed to be created." << L::EL;
                return false;
            }

            logg << L::SL << Color::GREEN << "[LedControl] Port #" << static_cast<int>(port) << " is ready." << L::EL;

            return true;
        }

        void LedControl::stop()
        {
            led_poll.reset();
            gpio_reset_pin(led_port);
        }

        void LedControl::blink(const unsigned times, const unsigned howlong)
        {
            std::lock_guard<std::mutex> l(mtx);
            led_poll.post([led = this->led_port,times,howlong] {
                for(unsigned p = 0; p < times; p++)
                {
                    gpio_set_level(led, true);
                    vTaskDelay(pdMS_TO_TICKS(howlong));
                    gpio_set_level(led, false);
                    vTaskDelay(pdMS_TO_TICKS(howlong / 2));
                }
            });            
        }

        void LedControl::force_high()
        {            
            std::lock_guard<std::mutex> l(mtx);
            led_poll.post([led = this->led_port] {
                gpio_set_level(led, true);
            });            
        }

        void LedControl::force_low()        
        {            
            std::lock_guard<std::mutex> l(mtx);
            led_poll.post([led = this->led_port] {
                gpio_set_level(led, false);
            });            
        }

    }
}