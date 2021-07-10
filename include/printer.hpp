/*
* # PRINTER
* Verified: 20210520
*/

#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <stdio.h>
#include <sstream>
#include <memory>

namespace LSW {
    namespace PocketDiscord {

        //constexpr size_t max_log_amount = 200;
        //constexpr size_t log_disk_handle_size = 32;
        constexpr size_t log_block_read_max_size = 128;
        constexpr size_t max_log_buf_size = 1536;
        //constexpr size_t max_each_log_block_size = 256;
        const std::string printer_path = "/sdcard/log/";

        enum class L{SL, EL};
        enum class Color {
            RED                     = 31,
            GREEN                   = 32,
            YELLOW                  = 33,
            BLUE                    = 34,
            MAGENTA                 = 35,
            CYAN                    = 36,
            WHITE                   = 37,
            BRIGHT_GRAY             = 90,
            BRIGHT_RED              = 91,
            BRIGHT_GREEN            = 92,
            BRIGHT_YELLOW           = 93,
            BRIGHT_BLUE             = 94,
            BRIGHT_MAGENTA          = 95,
            BRIGHT_CYAN             = 96,
            BRIGHT_WHITE            = 97,
            DEFAULT_COLOR           = 49
        };

        class Printer { // call like logg << L::SL << ... << L::L; (SL == StartLine, EL == EndLine)
            std::mutex mtx;
            std::string abusive_way;//, logging_buffer;
            std::string _buf_log;

            bool seek_block(const size_t, std::unique_ptr<FILE, std::function<void(FILE*)>>&); // returns true if there's data ahead of this
            std::unique_ptr<FILE, std::function<void(FILE*)>> make_file(const bool = false); // replace?
        public:
            bool log_at(const std::string&); // already adds "/sdcard/"

            std::string read_up_to(long); // in memory
            long log_size_now();

            Printer& operator<<(const L&);
            Printer& operator<<(const Color&);

            template<typename T>
            Printer& operator<<(const T&);
        };

        inline Printer logg;

        // - - - - IMPL - - - - //

        template<typename T>
        inline Printer& Printer::operator<<(const T& t)
        {
            std::cout << t;
            
            if (!abusive_way.empty()) {
                std::stringstream ss;
                ss << t;
                const auto strr = ss.str();

                _buf_log += strr;
                
                if (_buf_log.size() > max_log_buf_size){
                    size_t cutt = _buf_log.size() - max_log_buf_size;
                    _buf_log = _buf_log.substr(cutt);
                }

                auto fp = make_file();
                if (fp.get()){
                    for(size_t wrr = 0; wrr < strr.size();){
                        int rdd = fwrite(strr.data() + wrr, 1, strr.size() - wrr, fp.get());
                        if (rdd >= 0) wrr += static_cast<size_t>(rdd);
                        else break;
                    }
                }
            }
            
            return *this;
        }
    }
}