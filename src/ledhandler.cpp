#include "ledhandler.h"

struct __setup {
    __setup() {
        gpio_reset_pin(GPIO_NUM_2);
        gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    }
};


void set_led(const bool on)
{
    static __setup __st{};
    gpio_set_level(GPIO_NUM_2, on);
}