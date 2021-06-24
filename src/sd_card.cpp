#include "sd_card.hpp"

namespace LSW {
    namespace PocketDiscord {
        
        // call once
        bool mount_card()
        {
            esp_err_t ret;

            // Options for mounting the filesystem.
            // If format_if_mount_failed is set to true, SD card will be partitioned and
            // formatted in case when mounting fails.
            esp_vfs_fat_sdmmc_mount_config_t mount_config = {
                .format_if_mount_failed = false,
                .max_files = files_open_max,
                .allocation_unit_size = alloc_size_each
            };

            // Use settings defined above to initialize SD card and mount FAT filesystem.
            // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
            // Please check its source code and implement error recovery when developing
            // production applications.

            sdmmc_host_t host = SDSPI_HOST_DEFAULT();
            host.max_freq_khz = files_speed_hz;

            spi_bus_config_t bus_cfg = {
                .mosi_io_num = pin_mosi,
                .miso_io_num = pin_miso,
                .sclk_io_num = pin_clk,
                .quadwp_io_num = -1,
                .quadhd_io_num = -1,
                .max_transfer_sz = file_transf_size_limit,
            };

            ret = spi_bus_initialize(static_cast<spi_host_device_t>(host.slot), &bus_cfg, SPI_DMA_CHAN);
            if (ret != ESP_OK) {
                logg << L::SL << Color::RED << "[SD CARD] Failed to initialize bus." << L::EL;
                return false;
            }

            // This initializes the slot without card detect (CD) and write protect (WP) signals.
            // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
            sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
            slot_config.gpio_cs = pin_cs;
            slot_config.host_id = static_cast<spi_host_device_t>(host.slot);

            ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &__sd_global.card);

            if (ret != ESP_OK) {
                if (ret == ESP_FAIL) {
                    logg << L::SL << Color::RED << "[SD CARD] Failed to mount filesystem. If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option." << L::EL;
                } else {
                    logg << L::SL << Color::RED << "[SD CARD] Failed to initialize the card (" << esp_err_to_name(ret) << "). Make sure SD card lines have pull-up resistors in place." << L::EL;
                }
                return false;
            }

            // Card has been initialized, print its properties
            //sdmmc_card_print_info(stdout, __sd_global.card);
            __sd_global.host_slot = static_cast<spi_host_device_t>(host.slot);

            logg << L::SL << Color::GREEN << "[SD CARD] Card mounted!" << L::EL;

            return true;
        }

        void unmount_card()
        {
            if (__sd_global.card){
                esp_vfs_fat_sdcard_unmount(mount_point, __sd_global.card);

                logg << L::SL << Color::GREEN << "[SD CARD] Card unmounted!" << L::EL;

                //deinitialize the bus after all devices are removed
                spi_bus_free(__sd_global.host_slot);
                __sd_global.card = nullptr;
            }
        }

    }
}