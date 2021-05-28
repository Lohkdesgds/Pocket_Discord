#pragma once

#include <chrono>
#include <time.h>
#include <string>

namespace LSW {
    namespace PocketDiscord {

        struct time_iso {// GMT0
            unsigned year;
            unsigned month;
            unsigned day;
            unsigned hour;
            unsigned minute;
            float second;

            bool input(const std::string&); // format: 2020-09-23T18:15:46.827642+00:00
            bool input_epoch_ms(const int64_t);
            bool input_epoch_s(const int64_t);
            bool time_now();
            std::string iso_format() const;
            std::string nice_format() const;
        };

        unsigned long long get_time_now_ms();

    }
}