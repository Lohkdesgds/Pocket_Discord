#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_clk_tree.h"
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
        

        static const char TAG[] = u8"🤖 ⇒";
        
        volatile BotBase::sc_nvs*   BotBase::m_nvs = nullptr;
        volatile BotBase::sc_eloop* BotBase::m_eloop = nullptr;
        volatile BotBase::sc_sd*    BotBase::m_sd = nullptr;
        volatile BotBase::sc_wifi*  BotBase::m_wifi = nullptr;
        volatile BotBase::sc_ntp*   BotBase::m_ntp = nullptr;
        volatile size_t             BotBase::m_BotBases = 0;

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

        void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
        {
            wifi_status& cfg = *(wifi_status*)arg;

            switch(event_id) {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, u8"[🫐] WiFi connecting....");
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, u8"[🫐] WiFi connected!");
                cfg = wifi_status::CONNECTED;
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(TAG, u8"[🫐] WiFi lost connection");
                cfg = wifi_status::CONNECTING;
                esp_wifi_connect();
                ESP_LOGI(TAG, u8"[🫐] WiFi trying to connect again...");
                break;
            case IP_EVENT_STA_GOT_IP:
                ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
                ESP_LOGI(TAG, u8"[🫐] WiFi got IP: " IPSTR, IP2STR(&event->ip_info.ip));
                break;
            }
        }

        BotBase::sc_nvs::sc_nvs()
        {
            ESP_LOGI(TAG, u8"==================================================");
            ESP_LOGI(TAG, u8"[▶️] NVS FLASH INIT");
            auto err = nvs_flash_init();
            if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                ESP_ERROR_CHECK(nvs_flash_erase());
                err = nvs_flash_init();
            }
            ESP_ERROR_CHECK(err);
            ESP_LOGI(TAG, u8"[✅] NVS FLASH INIT DONE");
            ESP_LOGI(TAG, u8"==================================================");
        }
        BotBase::sc_nvs::~sc_nvs()
        {
            ESP_LOGI(TAG, u8"==================================================");
            ESP_LOGI(TAG, u8"[▶️] NVS FLASH DEINIT");
            nvs_flash_deinit();
            ESP_LOGI(TAG, u8"[✅] NVS FLASH DEINIT DONE");
            ESP_LOGI(TAG, u8"==================================================");
        }

        BotBase::sc_eloop::sc_eloop()
        {
            ESP_LOGI(TAG, u8"==================================================");
            ESP_LOGI(TAG, u8"[▶️] EVENT LOOP INIT");
            //ESP_ERROR_CHECK(esp_event_loop_create_default());
            e_hlr.start_global();
            ESP_LOGI(TAG, u8"[✅] EVENT LOOP INIT DONE");
            ESP_LOGI(TAG, u8"==================================================");
        }
        BotBase::sc_eloop::~sc_eloop()
        {
            ESP_LOGI(TAG, u8"==================================================");
            ESP_LOGI(TAG, u8"[▶️] EVENT LOOP DEINIT");
            //esp_event_loop_delete_default();
            e_hlr.destroy_global();
            ESP_LOGI(TAG, u8"[✅] EVENT LOOP DEINIT DONE");
            ESP_LOGI(TAG, u8"==================================================");
        }

        BotBase::sc_sd::sc_sd()
        {
            ESP_LOGI(TAG, u8"==================================================");
            ESP_LOGI(TAG, u8"[▶️] SD CARD INIT");

            esp_err_t ret;
            esp_vfs_fat_sdmmc_mount_config_t mount_config = {};
            mount_config.format_if_mount_failed = false;
            mount_config.max_files = MOUNT_FILE_MAX_OPEN_FILES_AT_ONCE;
            mount_config.allocation_unit_size = MOUNT_FILE_ALLOC_SIZE_EACH;
            
            sdmmc_host_t host = SDSPI_HOST_DEFAULT();
            host.max_freq_khz = MOUNT_FILE_SPEED_KHZ;
            spi_bus_config_t bus_cfg = {};
            bus_cfg.mosi_io_num = MOUNT_FILE_PIN_MOSI;
            bus_cfg.miso_io_num = MOUNT_FILE_PIN_MISO;
            bus_cfg.sclk_io_num = MOUNT_FILE_PIN_CLK;
            bus_cfg.quadwp_io_num = -1;
            bus_cfg.quadhd_io_num = -1;
            bus_cfg.max_transfer_sz = MOUNT_FILE_TRANSF_SIZE_LIMIT;

            ESP_LOGI(TAG, u8"[🔄️] Initializing bus...");

            ret = spi_bus_initialize(static_cast<spi_host_device_t>(host.slot), &bus_cfg, SPI_DMA_CHAN);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, u8"[❌] Failed to initialize bus.");
                eternal_flash_death(error_codes::SDCARD_BUS_FAILED);
            }
            
            sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
            slot_config.gpio_cs = MOUNT_FILE_PIN_CS;
            slot_config.host_id = static_cast<spi_host_device_t>(host.slot);
            
            ESP_LOGI(TAG, u8"[🔄️] Mounting...");

            ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
            if (ret != ESP_OK) {
                if (ret == ESP_FAIL) {
                    ESP_LOGE(TAG, u8"[❌] Failed to mount filesystem. If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
                    eternal_flash_death(error_codes::SDCARD_MOUNT_FAILED);
                } else {
                    ESP_LOGE(TAG, u8"[❌] Failed to initialize the card (%s). Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
                    eternal_flash_death(error_codes::SDCARD_MOUNT_FAILED);
                }
            }
            
            host_slot = static_cast<spi_host_device_t>(host.slot);
            
            ESP_LOGI(TAG, u8"[🔄️] Making file access available to all cores through events...");
            
            __file_make_event_handler_on_correct_core_id();
            
            ESP_LOGI(TAG, u8"[✅] SD CARD INIT DONE");
            ESP_LOGI(TAG, u8"==================================================");
        }
        BotBase::sc_sd::~sc_sd()
        {
            ESP_LOGI(TAG, u8"==================================================");
            ESP_LOGI(TAG, u8"[▶️] SD CARD DEINIT");
            if (card){
                esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
                
                spi_bus_free(host_slot);
                card = nullptr;
            }
            ESP_LOGI(TAG, u8"[✅] SD CARD DEINIT DONE");
            ESP_LOGI(TAG, u8"==================================================");
        }

        BotBase::sc_wifi::sc_wifi()
        {
            ESP_LOGI(TAG, u8"==================================================");
            ESP_LOGI(TAG, u8"[▶️] WIFI INIT");

            File fp("config.txt", "rb");
            
            wifi_config_t wifi_configuration{};
            char* your_ssid = (char*)wifi_configuration.sta.ssid;
            char* your_pass = (char*)wifi_configuration.sta.password;
            char key[16]{};
            size_t off = 0;
            if (fp.bad()) {
                ESP_LOGE(TAG, u8"[❌] config.txt failed to open.");
                ESP_LOGE(TAG, u8"[❌] Create it in root with WIFI_SSID= and WIFI_PWD= defined, like WIFI_SSID=YourSSID and WIFI_PWD=password123.");
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
                        ESP_LOGI(TAG, u8"[🔄️] Got WiFi SSID from config: %s", your_ssid);
                        off = 0;
                    }
                    else if (strcmp(key, "WIFI_PWD") == 0) {
                        while (!fp.eof() && (your_pass[off] = fp.getc()) != '\n' && your_pass[off] != '\r') {
                            if (your_pass[off] >= 32 && your_pass[off] <= 126) ++off;
                        }
                        your_pass[off] = '\0';
                        ESP_LOGI(TAG, u8"[🔄️] Got WiFi PWD from config: %s", your_pass);
                        off = 0;
                    }
                }
                else {
                    ++off;
                }
            }
            fp.close();

            ESP_LOGI(TAG, u8"[🔄️] Initializing WiFi...");

            esp_netif_init();
            esp_event_loop_create_default();
            m_netif_wifi_sta = (void*)esp_netif_create_default_wifi_sta();
            wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
            
            ESP_LOGI(TAG, u8"[🔄️] Hooking handler...");

            esp_wifi_init(&wifi_initiation);
            
            esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, (void*)&m_stat);
            esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, (void*)&m_stat);
            
            esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration);
            
            ESP_LOGI(TAG, u8"[🔄️] Starting WiFi...");

            esp_wifi_start();
            esp_wifi_set_mode(WIFI_MODE_STA);
            esp_wifi_connect();
            
            ESP_LOGI(TAG, u8"[🔄️] Waiting for WiFi to connect...");

            size_t c = 0;
            while(m_stat != wifi_status::CONNECTED) {
                vTaskDelay(pdMS_TO_TICKS(100));
                if (++c % 10 == 8) {
                    ESP_LOGI(TAG, u8"[🔄️] Still waiting for WiFi to connect...");
                    set_led(true);
                }
                else {
                    set_led(false);
                }
            }

            ESP_LOGI(TAG, u8"[✅] WIFI INIT DONE");
            ESP_LOGI(TAG, u8"==================================================");
        }
        BotBase::sc_wifi::~sc_wifi()
        {
            ESP_LOGI(TAG, u8"==================================================");
            ESP_LOGI(TAG, u8"[▶️] WIFI DEINIT");

            esp_wifi_disconnect();
            esp_wifi_stop();
            esp_wifi_deinit();
            esp_netif_destroy_default_wifi((void*)m_netif_wifi_sta);
            esp_netif_deinit();

            ESP_LOGI(TAG, u8"[✅] WIFI DEINIT DONE");
            ESP_LOGI(TAG, u8"==================================================");
        }

        BotBase::sc_ntp::sc_ntp()
        {
            ESP_LOGI(TAG, u8"==================================================");
            ESP_LOGI(TAG, u8"[▶️] NTP SERVICE INIT");

            esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
            esp_sntp_setservername(0, "a.st1.ntp.br");
            esp_sntp_init();
            {
                size_t c = 0;
                char buff[40]{};
                time_t now;
                struct tm timeinfo{};
                while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
                    vTaskDelay(pdMS_TO_TICKS(100));
                    if (++c % 20 == 8) {
                        ESP_LOGI(TAG, u8"[🔄️] Waiting for NTP to update time...");
                        set_led(true);
                    }
                    else {
                        set_led(false);
                    }
                    if (c > 600) {
                        ESP_LOGE(TAG, u8"[❌] NTP is bad.");
                        eternal_flash_death(error_codes::WIFI_NTP_FAILED);
                    }
                }
                setenv("TZ", "GMT3", 1);
                tzset();
                time(&now);
                localtime_r(&now, &timeinfo);
                
                strftime(buff, sizeof(buff), "%c", &timeinfo);
                ESP_LOGI(TAG, u8"[🔄️] Time in GMT-3: %s", buff);
            }
            ESP_LOGI(TAG, u8"[✅] NTP SERVICE INIT DONE");
            ESP_LOGI(TAG, u8"==================================================");
        }
        BotBase::sc_ntp::~sc_ntp()
        {
            ESP_LOGI(TAG, u8"==================================================");
            ESP_LOGI(TAG, u8"[▶️] NTP SERVICE DEINIT");
            esp_sntp_stop();
            ESP_LOGI(TAG, u8"[✅] NTP SERVICE DEINIT DONE");
            ESP_LOGI(TAG, u8"==================================================");
        }


    /// ==================== BotBase MANAGER CODE HERE ==================== ///


        BotBase::BotSelf::BotSelf(Gateway* g)
            : m_gateway(g)
        {}

        BotBase::BotSelf::~BotSelf()
        {
            DEL_IT(m_gateway);
        }

        Gateway* BotBase::BotSelf::gateway() const
        {
            return m_gateway;
        }

        BotBase::BotBase()
        {
            ESP_LOGI(TAG, "Starting BotBase...");

            if (get_cpu_clock() != 240000000) {
                ESP_LOGW(TAG, u8"==================================================");
                ESP_LOGW(TAG, "Clock is not at 240 MHz... Maybe you should change it to 240 MHz or more (if supported). Lagging startup by 10 seconds because of that.");
                ESP_LOGW(TAG, u8"==================================================");
                vTaskDelay(pdMS_TO_TICKS(10000));
            }

            set_led(true);
            vTaskDelay(pdMS_TO_TICKS(200));
            set_led(false);

            m_nvs = new sc_nvs();
            m_eloop = new sc_eloop();
            m_sd = new sc_sd();
            m_wifi = new sc_wifi();
            m_ntp = new sc_ntp();

            set_led(true);
        }

        BotBase::~BotBase()
        {
            destroy();
        }

        BotBase::BotSelf BotBase::make_bot(const char* token, const gateway_intents intents, const Gateway::event_handler function_handler)
        {
            return BotBase::BotSelf(new Gateway(token, intents, function_handler));
        }

        void BotBase::destroy()
        {
            if (m_ntp)   { delete m_ntp;   m_ntp = nullptr; }
            if (m_wifi)  { delete m_wifi;  m_wifi = nullptr; }
            if (m_sd)    { delete m_sd;    m_sd = nullptr; }
            if (m_eloop) { delete m_eloop; m_eloop = nullptr; }
            if (m_nvs)   { delete m_nvs;   m_nvs = nullptr; }
            set_led(false);
        }

        uint32_t BotBase::get_cpu_clock()
        {        
            uint32_t val;
            esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_CPU, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &val);
            return val;
        }

        ram_info BotBase::get_ram_info()
        {
            return ram_info();
        }
    }
}