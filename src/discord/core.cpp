#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_vfs_fat.h"
#include "esp_sntp.h"
#include "sdmmc_cmd.h"
#include <cstring>
#include <sys/unistd.h>
#include <sys/stat.h>

#include "discord/core.hpp"
#include "defaults.h"

#if CONFIG_IDF_TARGET_ESP32S2 ||CONFIG_IDF_TARGET_ESP32C3
#define SPI_DMA_CHAN    host.slot
#else
#define SPI_DMA_CHAN    1
#endif


namespace Lunaris {
    namespace PocketDiscord {

        static const char TAG[] = "PDC";
        
        struct sdcard_stuff{            
            sdmmc_card_t *card = nullptr;
            spi_host_device_t host_slot{};
        } __sd_global;

        void eternal_flash_death(const error_codes signal = error_codes::NONE)
        {
            while(1) {
                for(size_t c = 0; c < static_cast<size_t>(signal); c++) {
                    set_led(true);
                    vTaskDelay(pdMS_TO_TICKS(200));
                    set_led(false);
                    vTaskDelay(pdMS_TO_TICKS(800));
                }
                
                vTaskDelay(pdMS_TO_TICKS(5000));
            }
        }
        
        static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
        {
            bool* cfg = (bool*)arg;

            if(event_id == WIFI_EVENT_STA_START)
            {
                ESP_LOGI(TAG, "_ WiFi connecting....");
            }
            else if (event_id == WIFI_EVENT_STA_CONNECTED)
            {
                ESP_LOGI(TAG, "_ WiFi connected!");
                *cfg = true;
            }
            else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
            {
                ESP_LOGI(TAG, "_ WiFi lost connection");            
                esp_wifi_connect();
                ESP_LOGI(TAG, "_ WiFi trying to connect again...");
            }
            else if (event_id == IP_EVENT_STA_GOT_IP)
            {
                ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
                ESP_LOGI(TAG, "_ WiFi got IP: " IPSTR, IP2STR(&event->ip_info.ip));
            }
        }

        
        Bot::Bot()
        {
            ESP_LOGI(TAG, "Starting bot...");

            set_led(true);
            vTaskDelay(pdMS_TO_TICKS(200));
            set_led(false);

            ESP_LOGI(TAG, "# SETUP #");

            ESP_LOGI(TAG, "- NVS Flash Init...");
            auto err = nvs_flash_init();
            if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                ESP_ERROR_CHECK(nvs_flash_erase());
                err = nvs_flash_init();
            }
            ESP_ERROR_CHECK(err);

            ESP_LOGI(TAG, "- Event Loop Default...");
            ESP_ERROR_CHECK(esp_event_loop_create_default());

            ESP_LOGI(TAG, "- Basic settings ready!");
            
            ESP_LOGI(TAG, "# SD CARD #");
            
            esp_err_t ret;

            // Options for mounting the filesystem.
            // If format_if_mount_failed is set to true, SD card will be partitioned and
            // formatted in case when mounting fails.
            esp_vfs_fat_sdmmc_mount_config_t mount_config = {};
            mount_config.format_if_mount_failed = true;
            mount_config.max_files = MOUNT_FILE_MAX_OPEN_FILES_AT_ONCE;
            mount_config.allocation_unit_size = MOUNT_FILE_ALLOC_SIZE_EACH;

            // Use settings defined above to initialize SD card and mount FAT filesystem.
            // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
            // Please check its source code and implement error recovery when developing
            // production applications.

            sdmmc_host_t host = SDSPI_HOST_DEFAULT();
            host.max_freq_khz = MOUNT_FILE_SPEED_KHZ;

            spi_bus_config_t bus_cfg = {};
            bus_cfg.mosi_io_num = MOUNT_FILE_PIN_MOSI;
            bus_cfg.miso_io_num = MOUNT_FILE_PIN_MISO;
            bus_cfg.sclk_io_num = MOUNT_FILE_PIN_CLK;
            bus_cfg.quadwp_io_num = -1;
            bus_cfg.quadhd_io_num = -1;
            bus_cfg.max_transfer_sz = MOUNT_FILE_TRANSF_SIZE_LIMIT;

            
            ESP_LOGI(TAG, "- Initializing bus...");

            ret = spi_bus_initialize(static_cast<spi_host_device_t>(host.slot), &bus_cfg, SPI_DMA_CHAN);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to initialize bus.");
                eternal_flash_death(error_codes::SDCARD_BUS_FAILED);
                //while(1) vTaskDelay(200);
            }

            // This initializes the slot without card detect (CD) and write protect (WP) signals.
            // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
            sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
            slot_config.gpio_cs = MOUNT_FILE_PIN_CS;
            slot_config.host_id = static_cast<spi_host_device_t>(host.slot);
            
            ESP_LOGI(TAG, "- Mounting...");

            ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &__sd_global.card);

            if (ret != ESP_OK) {
                if (ret == ESP_FAIL) {
                    ESP_LOGE(TAG, "Failed to mount filesystem. If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
                    eternal_flash_death(error_codes::SDCARD_MOUNT_FAILED);
                    //while(1) vTaskDelay(200);
                } else {
                    ESP_LOGE(TAG, "Failed to initialize the card (%s). Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
                    eternal_flash_death(error_codes::SDCARD_MOUNT_FAILED);
                    //while(1) vTaskDelay(200);
                }
            }

            // Card has been initialized, print its properties
            //sdmmc_card_print_info(stdout, __sd_global.card);
            __sd_global.host_slot = static_cast<spi_host_device_t>(host.slot);

            ESP_LOGI(TAG, "- SD Card mounted!");

            ESP_LOGI(TAG, "# WIFI #");

            File fp("config.txt", "rb");
            //FILE* fp = fopen("/sdcard/config.txt", "rb");

            wifi_config_t wifi_configuration{};
            char* your_ssid = (char*)wifi_configuration.sta.ssid;
            char* your_pass = (char*)wifi_configuration.sta.password;
            char key[16]{};
            size_t off = 0;

            if (fp.bad()) {
                ESP_LOGE(TAG, "- config.txt failed to open.");
                ESP_LOGE(TAG, "- Create it in root with WIFI_SSID= and WIFI_PWD= defined, like WIFI_SSID=YourSSID and WIFI_PWD=password123.");
                eternal_flash_death(error_codes::SDCARD_CONFIG_FILE_NON_EXISTENT);
            }

            while(!fp.eof()) {
                key[off] = fp.getc();
                if (key[off] == '\r' || key[off] == '\n') continue;

                if (key[off] == '=') {
                    key[off] = '\0';
                    off = 0;

                    if (strcmp(key, "WIFI_SSID") == 0) {
                        while (!fp.eof() && (your_ssid[off] = fp.getc()) != '\n' && your_ssid[off] != '\r') {
                            if (your_ssid[off] >= 32 && your_ssid[off] <= 126) ++off;
                        }
                        your_ssid[off] = '\0';
                        ESP_LOGI(TAG, "- Got WiFi SSID from config: %s", your_ssid);
                        off = 0;
                    }
                    else if (strcmp(key, "WIFI_PWD") == 0) {
                        while (!fp.eof() && (your_pass[off] = fp.getc()) != '\n' && your_pass[off] != '\r') {
                            if (your_pass[off] >= 32 && your_pass[off] <= 126) ++off;
                        }
                        your_pass[off] = '\0';
                        ESP_LOGI(TAG, "- Got WiFi PWD from config: %s", your_pass);
                        off = 0;
                    }
                }
                else {
                    ++off;
                }
            }
            fp.close();

            ESP_LOGI(TAG, "- Initializing WiFi...");

            esp_netif_init(); //network interdace initialization
            esp_event_loop_create_default(); //responsible for handling and dispatching events
            esp_netif_create_default_wifi_sta(); //sets up necessary data structs for wifi station interface
            wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();//sets up wifi wifi_init_config struct with default values
            
            ESP_LOGI(TAG, "- Hooking handler...");

            esp_wifi_init(&wifi_initiation); //wifi initialised with dafault wifi_initiation

            volatile bool is_wifi_ready = false;

            esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, (void*)&is_wifi_ready); //creating event handler register for wifi
            esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, (void*)&is_wifi_ready); //creating event handler register for ip event
            //strcpy((char*)wifi_configuration.sta.ssid,your_ssid); // copy chars from hardcoded configs to struct
            //strcpy((char*)wifi_configuration.sta.password,your_pass);
            esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration);//setting up configs when event ESP_IF_WIFI_STA
            
            ESP_LOGI(TAG, "- Starting WiFi...");

            esp_wifi_start();//start connection with configurations provided in function
            esp_wifi_set_mode(WIFI_MODE_STA);//station mode selected
            esp_wifi_connect(); //connect with saved ssid and pass

            
            ESP_LOGI(TAG, "- Waiting for WiFi to connect...");

            size_t c = 0;
            while(!is_wifi_ready) {
                vTaskDelay(pdMS_TO_TICKS(100));
                if (++c % 10 == 8) {
                    ESP_LOGI(TAG, "- Still waiting for WiFi to connect...");
                    set_led(true);
                }
                else {
                    set_led(false);
                }
            }
            
            ESP_LOGI(TAG, "# NTP CONFIG #");

            ESP_LOGI(TAG, "- Setting up NTP time!");

            //esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("a.st1.ntp.br");
            //esp_netif_sntp_init(&config);
//
            //if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)) != ESP_OK) {
            //    ESP_LOGW(TAG, "- ST1 BR NTP does not work. Trying alt...");
//
            //    config = ESP_NETIF_SNTP_DEFAULT_CONFIG("b.st1.ntp.br");
            //    esp_netif_sntp_init(&config);
            //    if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)) != ESP_OK) {
            //        ESP_LOGW(TAG, "- ST1 BR NTP does not work. Trying default ESP NTP...");
            //        config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
            //        esp_netif_sntp_init(&config);
            //        
            //        if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)) != ESP_OK) {
            //            ESP_LOGE(TAG, "- Cannot setup NTP.");
            //            eternal_flash_death(error_codes::WIFI_NTP_FAILED);
            //        }
            //    }
            //}

            esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
            esp_sntp_setservername(0, "a.st1.ntp.br");
            esp_sntp_init();

            {
                c = 0;
                char buff[40]{};
                time_t now;
                struct tm timeinfo{};

                while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
                    vTaskDelay(pdMS_TO_TICKS(100));
                    if (++c % 20 == 8) {
                        ESP_LOGI(TAG, "- Waiting for NTP to update time...");
                        set_led(true);
                    }
                    else {
                        set_led(false);
                    }

                    if (c > 600) {
                        ESP_LOGE(TAG, "- NTP is bad.");
                        eternal_flash_death(error_codes::WIFI_NTP_FAILED);
                    }
                }

                setenv("TZ", "GMT3", 1);
                tzset();

                time(&now);
                localtime_r(&now, &timeinfo);
                
                strftime(buff, sizeof(buff), "%c", &timeinfo);
                ESP_LOGI(TAG, "Time in GMT-3: %s", buff);
            }

            ESP_LOGI(TAG, "- Done!");
            
            set_led(true);
        }

        Bot::~Bot()
        {
            if (__sd_global.card){
                esp_vfs_fat_sdcard_unmount(MOUNT_POINT, __sd_global.card);

                ESP_LOGI(TAG, "Card unmounted!");

                //deinitialize the bus after all devices are removed
                spi_bus_free(__sd_global.host_slot);
                __sd_global.card = nullptr;
            }
            set_led(false);
        }

    }
}