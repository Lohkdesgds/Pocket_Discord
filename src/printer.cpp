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

        std::string Printer::read_size(long plsread)
        {
            auto fp = make_file();
            if (!fp) return "";
            fseek(fp.get(), 0, SEEK_END);
            long len = ftell(fp.get());
            if (plsread > len) plsread = len;

            fseek(fp.get(), -plsread, SEEK_END);

            std::string aa;

            for(long u = 0; u < plsread;)
            {
                char blk[32];
                const size_t rdnow = (plsread - u > sizeof(blk)) ? sizeof(blk) : (plsread - u);
                int rdd = fread(blk, 1, rdnow, fp.get());
                if (rdd <= 0) return {};
                u += rdd;
                aa.append(blk, rdd);
            }
            return aa;
        }

        long Printer::log_size_now()
        {
            auto fp = make_file();
            if (!fp) return 0;
            fseek(fp.get(), 0, SEEK_END);
            return ftell(fp.get());
        }
        
        /*std::string Printer::read_buffer_line(const size_t index)
        {
            std::string buf;
            auto fp = make_file();
            if (!fp) return "<failed to read>";
            //fseek(fp.get(), 0, SEEK_END);
            //long endd = ftell(fp.get());
            size_t expect_b4 = ((index + 1) * log_block_read_max_size);

            if (fp.get() != nullptr)
            {
                fseek(fp.get(), - static_cast<long>(expect_b4), SEEK_END);
                for(size_t le = 0; le < log_block_read_max_size;) {

                    size_t curr_read = log_block_read_max_size - le;
                    if (curr_read > log_disk_handle_size) curr_read = log_disk_handle_size;

                    char ch[log_disk_handle_size];

                    int re = fread(ch, 1, curr_read, fp.get());
                    if (re <= 0) return "<invalid pos or failed to read>";
                    buf.append(ch, re);
                    le += re;
                }
            }
            return buf;
        }

        size_t Printer::log_size_now()
        {
            auto fp = make_file();
            if (!fp) return 0;
            fseek(fp.get(), 0, SEEK_END);
            return ftell(fp.get());
        }

        size_t Printer::log_size_blocks()
        {
            auto logsiz = log_size_now();
            if (logsiz == 0) return 0;
            return 1 + ((logsiz - 1) / log_block_read_max_size);
        }*/

        Printer& Printer::operator<<(const L& corner)
        {
            switch(corner){
            case L::SL:
                mtx.lock();
                break;
            case L::EL:
            {
                *this << Color::DEFAULT_COLOR << "\n";
                /*{
                    auto fp = make_file();
                    if (fp.get()){
                        fprintf(fp.get(), "\n");
                    }
                }*/
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