#pragma once

#include "nvs.h"
#include "nvs_flash.h"

#include "../heapstring.h"
#include "../filehandler.h"
#include "../functionwrapper.h"
#include "../ledhandler.h"
#include "../eventhandler.h"
#include "../useful.h"

#include "discord/gateway.hpp"

namespace Lunaris {
    namespace PocketDiscord {
        
        //const char mount_point[] = "/sdcard"; // don't ever change!

        enum class error_codes : size_t {
            NONE,
            SDCARD_BUS_FAILED,
            SDCARD_MOUNT_FAILED,
            SDCARD_CONFIG_FILE_NON_EXISTENT,
            WIFI_NTP_FAILED
        };

        enum class wifi_status : uint8_t {
            OFFLINE,
            CONNECTING,
            CONNECTED
        };

        class BotBase {
        public:
        
            class BotSelf {
                Gateway* m_gateway;
            public:
                BotSelf(Gateway*);
                ~BotSelf();
            };

        private:
            struct sc_nvs {
                sc_nvs();
                ~sc_nvs();
            };
            struct sc_eloop {
                EventHandlerDefault e_hlr;
                sc_eloop();
                ~sc_eloop();
            };
            struct sc_sd {
                sdmmc_card_t *card = nullptr;
                spi_host_device_t host_slot{};

                sc_sd();
                ~sc_sd();
            };
            struct sc_wifi {
                void* m_netif_wifi_sta = nullptr;
                wifi_status m_stat = wifi_status::OFFLINE;

                sc_wifi();
                ~sc_wifi();
            };
            struct sc_ntp {
                sc_ntp();
                ~sc_ntp();
            };

            static volatile sc_nvs*   m_nvs;
            static volatile sc_eloop* m_eloop;
            static volatile sc_sd*    m_sd;
            static volatile sc_wifi*  m_wifi;
            static volatile sc_ntp*   m_ntp;
            static volatile size_t    m_BotBases;
        public:
            BotBase();
            ~BotBase();

            static BotBase::BotSelf make_bot(const char* token, const gateway_intents intents, const Gateway::event_handler function_handler);

            void destroy();

            static uint32_t get_cpu_clock();
            static ram_info get_ram_info();
        };
    }
}