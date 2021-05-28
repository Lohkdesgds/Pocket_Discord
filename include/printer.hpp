/*
* # PRINTER
* Verified: 20210520
*/

#pragma once

#include <iostream>
#include <mutex>
#include <string>

namespace LSW {
    namespace PocketDiscord {

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

        class Printer { // call like logg << L::SL << ... << L::EL; (SL == StartLine, EL == EndLine)
            std::mutex mtx;
        public:
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
            return *this;
        }
    }
}