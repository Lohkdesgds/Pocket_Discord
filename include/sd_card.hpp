/*
* # SD_CARD
* Verified: < 20210520
*/

#pragma once

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

#include "printer.hpp"

#if CONFIG_IDF_TARGET_ESP32S2 ||CONFIG_IDF_TARGET_ESP32C3
#define SPI_DMA_CHAN    host.slot
#else
#define SPI_DMA_CHAN    1
#endif

namespace LSW {
    namespace PocketDiscord {
        
        const char mount_point[] = "/sdcard"; // don't ever change!
        constexpr int file_transf_size_limit = 4096;
        constexpr size_t alloc_size_each = 512;
        constexpr int files_open_max = 15;
        constexpr int files_speed_hz = 20000;

        constexpr gpio_num_t pin_miso = GPIO_NUM_19;
        constexpr gpio_num_t pin_mosi = GPIO_NUM_23;
        constexpr gpio_num_t pin_clk  = GPIO_NUM_18;
        constexpr gpio_num_t pin_cs   = GPIO_NUM_5;

        struct global_stuff{            
            sdmmc_card_t *card = nullptr;
            spi_host_device_t host_slot{};
        };

        inline global_stuff __sd_global;

        bool mount_card();
        void unmount_card();

    }
}