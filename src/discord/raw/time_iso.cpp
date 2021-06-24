#include "discord/raw/time_iso.hpp"

namespace LSW {
    namespace PocketDiscord {

        bool time_iso::input(const std::string& iso) // format: 2020-09-23T18:15:46.827642+00:00
        {
            if (iso.empty()) return false;
            sscanf(iso.c_str(), "%u-%u-%uT%u:%u:%f", &year, &month, &day, &hour, &minute, &second);
            return true;
        }

        bool time_iso::input_epoch_ms(const int64_t dt)
        {
            time_t temp = dt / 1e3;
            struct tm* tn_ptr = gmtime(&temp);
            if (!tn_ptr) return false;
            struct tm& tn = *tn_ptr; // solved

            year = tn.tm_year + 1900;
            month = tn.tm_mon + 1;
            day = tn.tm_mday;
            hour = tn.tm_hour;
            minute = tn.tm_min;
            second = (float)(tn.tm_sec) + (float)((dt % 1000) * 1e-3f);

            return true;
        }

        bool time_iso::input_epoch_s(const int64_t t)
        {
            return input_epoch_ms(t * 1000ull);
        }

        bool time_iso::time_now()
        {
            struct timeval tv_now;
            gettimeofday(&tv_now, NULL);
            
            struct tm* tn_ptr = gmtime(&tv_now.tv_sec);
            if (!tn_ptr) return false;
            struct tm& tn = *tn_ptr; // solved

            year = tn.tm_year + 1900;
            month = tn.tm_mon + 1;
            day = tn.tm_mday;
            hour = tn.tm_hour;
            minute = tn.tm_min;
            second = (float)(tn.tm_sec) + (float)((tv_now.tv_usec) * 1e-6f);

            return true;
        }

        std::string time_iso::iso_format() const
        {
            char buf[48];
            sprintf(buf, "%04u-%02u-%02uT%02u:%02u:%09.6f+00:00", year, month, day, hour, minute, second);
            return buf;
        }

        std::string time_iso::nice_format() const
        {
            char buf[64];
            sprintf(buf, "%04u/%02u/%02u %02u:%02u:%04.1f", year, month, day, hour, minute, second);
            return buf;
        }

        unsigned long long get_time_now_ms()
        {
            return std::chrono::duration_cast<std::chrono::duration<unsigned long long, std::milli>>(std::chrono::system_clock::now().time_since_epoch()).count();
        }
    }
}