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

        /*struct start_block { // 256 bytes wide
            size_t current_start = 0;
            char __padd[256 - sizeof(size_t)];
        };

        struct log_block {
            char data[log_block_size]{};

            void set(const std::string&);
            std::string get();
        };*/

        class Printer { // call like logg << L::SL << ... << L::EL; (SL == StartLine, EL == EndLine)
            std::mutex mtx;
            std::string abusive_way;//, logging_buffer;

            bool seek_block(const size_t, std::unique_ptr<FILE, std::function<void(FILE*)>>&); // returns true if there's data ahead of this
            std::unique_ptr<FILE, std::function<void(FILE*)>> make_file(const bool = false); // replace?
        public:
            bool log_at(const std::string&); // already adds "/sdcard/"

            std::string read_size(long);
            long log_size_now();
            /*size_t log_size_now();
            size_t log_size_blocks();*/

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
            /*if (!abusive_way.empty()){
                auto fp = make_file();
                if (fp.get()){
                    std::stringstream ss;
                    ss << t;
                    const auto str = ss.str();
                    std::cout << "{" << fwrite(str.data(), sizeof(char), str.size(), fp.get()) << "}";
                }
            }*/
            if (!abusive_way.empty()) {
                std::stringstream ss;
                ss << t;
                const auto strr = ss.str();
                //logging_buffer += ss.str();//.substr(0, max_each_log_block_size);

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