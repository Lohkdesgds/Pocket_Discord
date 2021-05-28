#include "printer.hpp"

namespace LSW {
    namespace PocketDiscord {
        
        Printer& Printer::operator<<(const L& corner)
        {
            switch(corner){
            case L::SL:
                mtx.lock();
                break;
            case L::EL:
                *this << Color::DEFAULT_COLOR << "\n";
                mtx.unlock();
                break;
            }

            return *this;
        }
        
        Printer& Printer::operator<<(const Color& color)
        {
            std::cout << "\033[" << static_cast<int>(color) << "m";

            return *this;
        }

    }
}