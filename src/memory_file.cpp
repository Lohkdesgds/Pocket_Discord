#include "memory_file.hpp"

namespace LSW {
    namespace PocketDiscord {

        unsigned MemoryFile::fpcount = 0;
        unsigned MemoryFile::fpopened = 0;
        std::mutex MemoryFile::safemtx;

        void MemoryFile::gen_new()
        {
            close_fp();
            {
                std::lock_guard<std::mutex> lucky(safemtx);

                for (int u = 0; u < 3 && !fp; u++) {
                    fp = fopen((memory_path + std::to_string(fpcount++)).c_str(), "wb+");
                    if (!fp) logg << L::SL << Color::YELLOW << "[MemoryFile] MemoryFile failed to open once [" << (u + 1) << "/3]" << L::EL;
                }

                tempbuf.resize(memfile_tempbuf_size, '\0');

                if (fpcount > 100) fpcount = 0;
                if (++fpopened >= (files_open_max - 1)) {
                    logg << L::SL << Color::YELLOW << "[MemoryFile] MemoryFiles are close to the limit!" << L::EL;
                }
            }
            if (!fp) {
                logg << L::SL << Color::RED << "[MemoryFile] Failed to open temporary file!" << L::EL;
                throw std::runtime_error("[MemoryFile] Failed to open temporary file! At file N=" + std::to_string(fpcount) + " (resets at 100). Aborting.");
            }
        }

        void MemoryFile::close_fp()
        {
            if (fp) {
                fclose(fp);
                fp = nullptr;
                tempbuf.clear();
                std::lock_guard<std::mutex> lucky(safemtx);
                if (fpopened) fpopened--;
            }
        }

        void MemoryFile::check_gen_new()
        {
            if (!fp) gen_new();
        }

        MemoryFile::MemoryFile()
        {
            //gen_new();
        }

        MemoryFile::~MemoryFile()
        {
            if (fp) {
                close_fp();
            }
        }

        MemoryFile::MemoryFile(const MemoryFile& mf)
        {
            check_gen_new();
            for (size_t p = 0; p < mf.size(); p++) push_back(mf[p]);
            size_now_valid = mf.size_now_valid;
        }

        MemoryFile::MemoryFile(MemoryFile&& mf) noexcept
        {
            fp = mf.fp;

    #ifndef LSW_MEMORYFILE_NOBUFFER
            /*for (size_t p = 0; p < memfile_tempbuf_size; p++) {
                tempbuf[p] = mf.tempbuf[p];
                mf.tempbuf[p] = '\0';
            }*/
            tempbuf = std::move(mf.tempbuf);
    #endif

            size_now_valid = mf.size_now_valid;
            mf.size_now_valid = 0;
            mf.fp = nullptr;
        }

        void MemoryFile::operator=(const MemoryFile& mf)
        {
            check_gen_new();
            for (size_t p = 0; p < mf.size(); p++) push_back(mf[p]);
            size_now_valid = mf.size_now_valid;
        }

        void MemoryFile::operator=(MemoryFile&& mf) noexcept
        {
            fp = mf.fp;

    #ifndef LSW_MEMORYFILE_NOBUFFER
            /*for (size_t p = 0; p < memfile_tempbuf_size; p++) {
                tempbuf[p] = mf.tempbuf[p];
                mf.tempbuf[p] = '\0';
            }*/
            tempbuf = std::move(mf.tempbuf);
    #endif

            size_now_valid = mf.size_now_valid;
            mf.size_now_valid = 0;
            mf.fp = nullptr;
        }

        bool MemoryFile::operator==(const MemoryFile& oth) const
        {
            return fp == oth.fp;
        }

        bool MemoryFile::operator!=(const MemoryFile& oth) const
        {
            return fp != oth.fp;
        }

        unsigned MemoryFile::files_open() const
        {
            return fpopened;
        }

        size_t MemoryFile::size() const
        {
            return fp ? size_now_valid : 0;
        }

        size_t MemoryFile::length() const
        {
            return size();
        }

        void MemoryFile::resize(const size_t off)
        {
            check_gen_new();
            size_now_valid = off;
        }

        void MemoryFile::reserve(const size_t siz)
        {
            resize(siz); // does check_gen_new();
        }

        void MemoryFile::clear()
        {
            size_now_valid = 0;
            close_fp();
        }

        bool MemoryFile::empty() const
        {
            return fp == nullptr || size_now_valid == 0;
        }

        bool MemoryFile::valid() const
        {
            return !empty();
        }

        char MemoryFile::operator[](const size_t at) const
        {
            if (!fp) return '\0';
            if (at >= size_now_valid) return '\0';

    #ifndef LSW_MEMORYFILE_NOBUFFER
            if (at < memfile_tempbuf_size) return tempbuf[at];
            fseek(fp, static_cast<long>(at - memfile_tempbuf_size), SEEK_SET);
    #else
            fseek(fp, static_cast<long>(at), SEEK_SET);
    #endif

            int res = fgetc(fp);
            if (res > 0) return static_cast<char>(res);
            return '\0';
        }

        char MemoryFile::at(const size_t at) const
        {
            if (!fp) return '\0';
            if (at >= size_now_valid) return '\0';

    #ifndef LSW_MEMORYFILE_NOBUFFER
            if (at < memfile_tempbuf_size) return tempbuf[at];
            fseek(fp, static_cast<long>(at - memfile_tempbuf_size), SEEK_SET);
    #else
            fseek(fp, static_cast<long>(at), SEEK_SET);
    #endif

            int res = fgetc(fp);
            if (res > 0) return static_cast<char>(res);
            return '\0';
        }

        char MemoryFile::back() const
        {
            if (!fp) return '\0';
            if (size_now_valid == 0) return '\0';

    #ifndef LSW_MEMORYFILE_NOBUFFER
            if (size_now_valid < memfile_tempbuf_size) return tempbuf[size_now_valid - 1];
            fseek(fp, static_cast<long>(size_now_valid - memfile_tempbuf_size - 1), SEEK_SET);
    #else
            fseek(fp, static_cast<long>(size_now_valid - 1), SEEK_SET);
    #endif

            int res = fgetc(fp);
            if (res > 0) return static_cast<char>(res);
            return '\0';
        }

        char MemoryFile::front() const
        {
            if (!fp) return '\0';
            if (size_now_valid == 0) return '\0';

    #ifndef LSW_MEMORYFILE_NOBUFFER
            return tempbuf[0];
    #else
            fseek(fp, 0, SEEK_SET);
            int res = fgetc(fp);
            if (res > 0) return static_cast<char>(res);
            return '\0';
    #endif
        }

        void MemoryFile::operator=(const std::string& str)
        {
            check_gen_new();
            size_now_valid = str.length();

    #ifndef LSW_MEMORYFILE_NOBUFFER
            for (size_t p = 0; p < memfile_tempbuf_size && p < size_now_valid; p++) tempbuf[p] = str[p];
            if (size_now_valid > memfile_tempbuf_size) {
                fseek(fp, 0, SEEK_SET);
                int errs = 0;
                for (size_t p = 0; p < str.length() - memfile_tempbuf_size;) {
                    size_t wrt = fwrite(str.data() + memfile_tempbuf_size + p, sizeof(char), str.length() - memfile_tempbuf_size - p, fp);
                    if (wrt == 0) {
                        if (++errs > 3) {
    #ifndef LSW_DISCARD_READWRITE_ERRORS
                            logg << L::SL << Color::YELLOW << "[MemoryFile] Failed to write 3 times, leaving this as is." << L::EL;
    #endif
                            break;
                        }
                    }
                    else { errs = 0; p += wrt; }
                }
            }
    #else
            fseek(fp, 0, SEEK_SET);
            int errs = 0;
            for (size_t p = 0; p < str.length();) {
                size_t wrt = fwrite(str.data() + p, sizeof(char), str.length() - p, fp);
                if (wrt == 0) {
                    if (++errs > 3) {
    #ifndef LSW_DISCARD_READWRITE_ERRORS
                        logg << L::SL << Color::YELLOW << "[MemoryFile] Failed to write 3 times, leaving this as is." << L::EL;
    #endif
                        break;
                    }
            }
                else { errs = 0; p += wrt; }
        }
    #endif
    }

        void MemoryFile::operator+=(const std::string& str)
        {
            check_gen_new();

    #ifndef LSW_MEMORYFILE_NOBUFFER
            size_t str_pos = 0;

            for (size_t p = size_now_valid; p < memfile_tempbuf_size && str_pos < str.length(); p++) tempbuf[p] = str[str_pos++];

            if (str_pos < str.length()) {
                fseek(fp, static_cast<long>(size_now_valid - memfile_tempbuf_size), SEEK_SET);
                int errs = 0;
                for (size_t p = 0; p < str.length() - str_pos;) {
                    size_t wrt = fwrite(str.data() + str_pos + p, sizeof(char), str.length() - str_pos - p, fp);
                    if (wrt == 0) {
                        if (++errs > 3) {
    #ifndef LSW_DISCARD_READWRITE_ERRORS
                            logg << L::SL << Color::YELLOW << "[MemoryFile] Failed to write 3 times, leaving this as is." << L::EL;
    #endif
                            break;
                        }
                    }
                    else { errs = 0; p += wrt; }
                }
            }
    #else
            fseek(fp, static_cast<long>(size_now_valid), SEEK_SET);

            int errs = 0;
            for (size_t p = 0; p < str.length();) {
                size_t wrt = fwrite(str.data() + p, sizeof(char), str.length() - p, fp);
                if (wrt == 0) {
                    if (++errs > 3) {
    #ifndef LSW_DISCARD_READWRITE_ERRORS
                        logg << L::SL << Color::YELLOW << "[MemoryFile] Failed to write 3 times, leaving this as is." << L::EL;
    #endif
                        break;
                    }
                }
                else { errs = 0; p += wrt; }
            }
    #endif

            size_now_valid += str.length();
                        }

        void MemoryFile::append(const std::string& str)
        {
            *this += str;
        }

        void MemoryFile::append(const char* buf, const size_t siz)
        {
            std::string str(buf, siz);
            *this += str;
        }

        void MemoryFile::push_back(const char ch)
        {
            check_gen_new();

    #ifndef LSW_MEMORYFILE_NOBUFFER
            if (size_now_valid < memfile_tempbuf_size) {
                tempbuf[size_now_valid++] = ch;
                return;
            }
            fseek(fp, static_cast<long>(size_now_valid - memfile_tempbuf_size), SEEK_SET);
            fputc(ch, fp);
    #else
            fseek(fp, static_cast<long>(size_now_valid), SEEK_SET);
            fputc(ch, fp);
    #endif

            size_now_valid++;
                    }

        void MemoryFile::assign(const std::string& str)
        {
            *this = str;
        }

        void MemoryFile::assign(const char* buf, const size_t siz)
        {
            std::string str(buf, siz);
            *this = str;
        }

        void MemoryFile::pop_back()
        {
            if (size_now_valid) size_now_valid--;
        }

        void MemoryFile::swap(MemoryFile&& mf)
        {
            FILE* _temp = fp;
            size_t _siz = size_now_valid;

            fp = mf.fp;
            size_now_valid = mf.size_now_valid;

            mf.fp = _temp;
            mf.size_now_valid = _siz;

    #ifndef LSW_MEMORYFILE_NOBUFFER
            for (size_t p = 0; p < memfile_tempbuf_size; p++) {
                char _temp_cpy = tempbuf[p];
                tempbuf[p] = mf.tempbuf[p];
                mf.tempbuf[p] = _temp_cpy;
            }
    #endif
        }

        size_t MemoryFile::find_after(const char ch, const size_t aft) const
        {
            if (!fp) return static_cast<size_t>(-1);
            if (aft > size_now_valid) return static_cast<size_t>(-1);
            for (size_t p = aft; p < size_now_valid; p++) {
                if (at(p) == ch) return p;
            }
            return static_cast<size_t>(-1);
        }

        size_t MemoryFile::find_before(const char ch, const size_t bef) const
        {
            if (!fp) return static_cast<size_t>(-1);
            if (bef > size_now_valid || bef == 0) return static_cast<size_t>(-1);
            for (size_t p = bef; p > 0; p--) {
                if (at(p) == ch && (p == 0 || at(p-1) != '\\')) return p;
            }
            if (at(0) == ch) return 0;
            return static_cast<size_t>(-1);
        }

        bool MemoryFile::find_block(size_t& start, size_t& endd, const size_t beg, const size_t lim, const char str, const char enf) const
        {
            const size_t real_lim = lim > size() ? size() : lim;//lim >= m_end ? (m_end > memfp.size() ? memfp.size() : m_end) : lim;
            if (real_lim <= beg) return false;

            int points = 0;
            bool block_ch = false; // this thing: backslash
            for (size_t p = beg; p < real_lim; p++) {

                char att = at(p);

                if (att == '\\') {
                    block_ch = true;
                    continue;
                }
                else if (block_ch) {
                    block_ch = false;
                    continue;
                }

                if (enf == '\0' && att == str) { // find first case only
                    endd = p;
                    start = beg;
                    return true;
                }
                if (str == enf && points > 0 && att == str) { // at least one, so like "string" has 2 cases
                    endd = p;
                    return true;
                }
                else if (att == str) {
                    if (points++ == 0) start = p;
                }
                else if (att == enf) {
                    if (--points == 0) {
                        endd = p;
                        return true;
                    }
                }
            }

            return false;
        }

        std::string MemoryFile::substr(const size_t beg, const size_t siz) const
        {
            if (!fp) return "";

            const size_t totallen = (siz == static_cast<size_t>(-1)) ? size_now_valid : (((siz > size_now_valid) || ((siz + beg) > size_now_valid)) ? size_now_valid : (siz));

            std::string res;
            res.resize(totallen);

            // Here I was trying something faster, but right now it's better "perfect" than "a little faster".
//#ifndef LSW_MEMORYFILE_NOBUFFER
            for (size_t p = 0; p < totallen; p++) res[p] = at(p + beg);
/*#else
            int errs = 0;
            fseek(fp, static_cast<long>(beg), SEEK_SET);

            for (size_t total = 0; total < totallen;)
            {
                size_t read_curr = ((totallen - total) > read_block_chunk_size) ? read_block_chunk_size : (totallen - total);
                int read_now = fread(res.data() + total, sizeof(char), read_curr, fp);
                if (read_now <= 0) { // go with what you have (this may try to read out of bounds anyway)
                    if (++errs > 2) {
#ifndef LSW_DISCARD_READWRITE_ERRORS
                        logg << L::SL << Color::YELLOW << "[MemoryFile] Failed to read 2 times, leaving this as is." << L::EL;
#endif
                        break;
                    }
                }
                else {
                    errs = 0;
                    total += read_now;
                }
            }
#endif*/

            return res;
        }

    }
}