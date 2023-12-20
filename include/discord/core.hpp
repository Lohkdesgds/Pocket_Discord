#pragma once

#include "heapstring.h"
#include "filehandler.h"
#include "functionwrapper.h"
#include "ledhandler.h"

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
        
        class Bot {
        public:
            Bot();
            ~Bot();
        };
    }
}