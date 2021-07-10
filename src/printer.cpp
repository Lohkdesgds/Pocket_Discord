#include "printer.hpp"

namespace LSW {
    namespace PocketDiscord {
        
        std::unique_ptr<FILE, std::function<void(FILE*)>> Printer::make_file(const bool newfile) {
            return std::unique_ptr<FILE, std::function<void(FILE*)>>([&] {
                FILE* f = nullptr;
                for (size_t errs = 0; !f; errs++) {
                    if (errs >= 5) {
                        return (FILE*)nullptr;
                    }
                    f = fopen(abusive_way.c_str(), (newfile ? "wb+" : "ab+"));
                }
                return f;
                }(), [](FILE* f) { if (f) fclose(f); });
        }
        
        bool Printer::log_at(const std::string& path)
        {
            std::lock_guard<std::mutex> luck(mtx);

            abusive_way = (printer_path + path);
            auto fp = make_file(true);

            return fp.get() != nullptr;
        }

        std::string Printer::read_up_to(long plsread)
        {
            return _buf_log.substr(0, plsread);
        }

        long Printer::log_size_now()
        {
            auto fp = make_file();
            if (!fp) return 0;
            fseek(fp.get(), 0, SEEK_END);
            return ftell(fp.get());
        }

        Printer& Printer::operator<<(const L& corner)
        {
            switch(corner){
            case L::SL:
                mtx.lock();
                break;
            case L::EL:
            {
                *this << Color::DEFAULT_COLOR << "\n";
                
                mtx.unlock();
            }
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