#include "memory_file.hpp"

namespace LSW {
    namespace PocketDiscord {

        MemoryFile::_shared MemoryFile::s_shared;

        unsigned MemoryFile::_shared::request()
        {
            std::lock_guard<std::mutex> l(mtx);
            m_count = (m_count + 1) % memfile_max_file_number;
            return m_count;
        }

        void MemoryFile::set_size(const size_t max_len)
        {
            if (max_len > m_ram_size) {
                if (!m_fp) {
                    m_fp = std::move(std::unique_ptr<FILE, std::function<void(FILE*)>>([&] {
                        std::string p = memory_path + std::to_string(s_shared.request());
                        FILE* f = nullptr;
                        for (size_t errs = 0; !f; errs++) {
                            if (errs >= 5) {
                                logg << L::SL << Color::RED << "[MemoryFile] Failed to open file! Fatal error!" << L::EL;
                                throw std::runtime_error("[MemoryFile] Can't open new file for buffering");
                            }
                            f = fopen(p.c_str(), "wb+");
                        }
                        return f;
                        }(), [](FILE* f) { fclose(f); }));
                }
            }
            else if (m_fp) {
                m_fp.reset();
            }

            if (max_len > 0) {
                const size_t max_for_ram = max_len > max_ram() ? max_ram() : max_len;
                m_buf.resize(max_for_ram);
            }
            else m_buf.clear();

            m_size_now = max_len;
        }

        void MemoryFile::write_noalloc(const char* ch, size_t siz, const size_t off)
        {
            /*if (off + siz > max_ram() && m_fp.get()) fseek(m_fp.get(), 0, SEEK_SET);

            for (size_t p = 0; p < siz; p++) {
                const size_t pos = p + off;
                if (pos >= m_size_now) break;
                if (pos < max_ram()) m_buf[pos] = ch[p];
                else fwrite(ch + p, sizeof(char), 1, m_fp.get());
            }*/

            size_t writable_ram = (max_ram() > off) ? (max_ram() - off) : 0;
            if (writable_ram > siz) writable_ram = siz;

            for (size_t p = 0; p < writable_ram; p++) m_buf[p + off] = ch[p];
            siz -= writable_ram;

            if (siz > 0) fseek(m_fp.get(), static_cast<long>((off > max_ram() ? (off - max_ram()) : 0)), SEEK_SET);

            unsigned zeros = 0;
            while (siz > 0) {
                const size_t to_write = siz > memfile_blocks_copy ? memfile_blocks_copy : siz;
                size_t _writt = 0;
                while ((_writt = fwrite(ch + writable_ram, sizeof(char), to_write, m_fp.get())) == 0) {
                    if (++zeros >= 10) {
                        logg << L::SL << Color::RED << "[MemoryFile] Failed to write 10 times, leaving this as is." << L::EL;
                        return;
                    }
                }
                if (_writt <= siz) siz -= _writt;
                else siz = 0;
            }
        }

        std::string MemoryFile::read_noalloc(size_t siz, const size_t off) const
        {
            /*std::string cpy;
            for (size_t p = 0; p < siz; p++) {
                if (p + off >= m_size_now) break;
                cpy += at(off + p);
            }
            return cpy;*/

            if (off + siz > m_size_now)
                siz = m_size_now - off;

            std::string out;

            size_t readable_ram = (off > max_ram()) ? 0 : (max_ram() - off);
            if (readable_ram > siz) readable_ram = siz;

            if (readable_ram > 0) out = std::string(m_buf.begin() + off, m_buf.begin() + off + readable_ram);
            siz -= readable_ram;

            if (siz > 0) fseek(m_fp.get(), static_cast<long>((off > max_ram() ? (off - max_ram()) : 0)), SEEK_SET);

            unsigned zeros = 0;
            for (size_t p = 0; p < siz;) {
                char _temp[memfile_blocks_copy];
                const size_t to_read = siz > memfile_blocks_copy ? memfile_blocks_copy : siz;
                size_t _read = 0;

                while ((_read = fread(_temp, sizeof(char), to_read, m_fp.get())) == 0) {
                    if (++zeros >= 10) {
                        logg << L::SL << Color::RED << "[MemoryFile] Failed to read 10 times, returning what was available right now." << L::EL;
                        return out;
                    }

                }
                out.append(_temp, _read);
                p += _read;
            }

            return out;
        }

        size_t MemoryFile::max_ram() const
        {
            return m_ram_size;
        }

        MemoryFile::MemoryFile(const size_t ramsiz)
            : m_ram_size(ramsiz)
        {
        }

        MemoryFile::MemoryFile(const MemoryFile& oth)
        {
            set_size(oth.size());
            for (size_t p = 0; p < oth.size(); p += memfile_blocks_copy) {
                const std::string _str = oth.substr(p, memfile_blocks_copy);
                write_noalloc(_str.data(), _str.size(), p);
            }
        }

        MemoryFile::MemoryFile(MemoryFile&& oth) noexcept
            : m_fp(std::move(oth.m_fp)), m_size_now(oth.m_size_now), m_ram_size(oth.m_ram_size), m_buf(std::move(oth.m_buf))
        {
            oth.set_size(0);
        }

        MemoryFile::MemoryFile(const std::string& str)
        {
            *this = str;
        }

        void MemoryFile::operator=(const MemoryFile& oth)
        {
            set_size(oth.size());
            for (size_t p = 0; p < oth.size(); p += memfile_blocks_copy) {
                const std::string _str = oth.substr(p, memfile_blocks_copy);
                write_noalloc(_str.data(), _str.size(), p);
            }
        }

        void MemoryFile::operator=(MemoryFile&& oth) noexcept
        {
            m_fp = std::move(oth.m_fp);
            m_size_now = oth.m_size_now;
            m_ram_size = oth.m_ram_size;
            m_buf = std::move(oth.m_buf);

            oth.set_size(0);
        }

        bool MemoryFile::operator==(const MemoryFile& oth) const
        {
            return m_fp.get() == oth.m_fp.get() && m_size_now == oth.m_size_now && m_ram_size == oth.m_ram_size && m_buf == oth.m_buf;
        }

        bool MemoryFile::operator!=(const MemoryFile& oth) const
        {
            return !(*this == oth);
        }

        size_t MemoryFile::size() const
        {
            return m_size_now;
        }

        size_t MemoryFile::length() const
        {
            return m_size_now;
        }

        void MemoryFile::resize(const size_t new_lim)
        {
            set_size(new_lim);
        }

        void MemoryFile::reserve(const size_t new_lim)
        {
            set_size(new_lim);
        }

        void MemoryFile::clear()
        {
            set_size(0);
        }

        bool MemoryFile::empty() const
        {
            return m_size_now == 0;
        }

        bool MemoryFile::valid() const
        {
            return !empty();
        }

        char MemoryFile::operator[](const size_t at) const
        {
            if (at >= m_size_now) return '\0';
            if (at < max_ram()) return m_buf[at];
            fseek(m_fp.get(), static_cast<long>(at - max_ram()), SEEK_SET);
            int ch = fgetc(m_fp.get());
            if (ch > 0) return ch;
            return '\0';
        }

        char MemoryFile::at(const size_t at) const
        {
            if (at >= m_size_now) return '\0';
            if (at < max_ram()) return m_buf[at];
            fseek(m_fp.get(), static_cast<long>(at - max_ram()), SEEK_SET);
            int ch = fgetc(m_fp.get());
            if (ch > 0) return ch;
            return '\0';
        }

        char MemoryFile::back() const
        {
            if (m_size_now == 0) return '\0';
            return at(m_size_now - 1);
        }

        char MemoryFile::front() const
        {
            return at(0);
        }

        void MemoryFile::operator=(const std::string& str)
        {
            set_size(str.size());

            for (size_t p = 0; p < str.size(); p += memfile_blocks_copy) {
                const std::string _str = str.substr(p, memfile_blocks_copy);
                write_noalloc(_str.data(), _str.size(), p);
            }
        }

        void MemoryFile::operator+=(const std::string& str)
        {
            const size_t offset = m_size_now;
            set_size(m_size_now + str.size());

            for (size_t p = 0; p < str.size(); p += memfile_blocks_copy) {
                const std::string _str = str.substr(p, memfile_blocks_copy);
                write_noalloc(_str.data(), _str.size(), p + offset);
            }
        }

        void MemoryFile::append(const std::string& str)
        {
            const size_t offset = m_size_now;
            set_size(m_size_now + str.size());

            for (size_t p = 0; p < str.size(); p += memfile_blocks_copy) {
                const std::string _str = str.substr(p, memfile_blocks_copy);
                write_noalloc(_str.data(), _str.size(), p + offset);
            }
        }

        void MemoryFile::append(const char* str, const size_t siz)
        {
            const size_t offset = m_size_now;
            set_size(m_size_now + siz);

            for (size_t p = 0; p < siz; p += memfile_blocks_copy) {
                const size_t curr_siz = (((siz - p) < memfile_blocks_copy) ? (siz - p) : memfile_blocks_copy);
                write_noalloc(str + p, curr_siz, p + offset);
            }
        }

        void MemoryFile::push_back(const char ch)
        {
            const size_t pos = m_size_now;
            set_size(m_size_now + 1);
            write_noalloc(&ch, 1, pos);
        }

        void MemoryFile::assign(const std::string& str)
        {
            set_size(str.size());

            for (size_t p = 0; p < str.size(); p += memfile_blocks_copy) {
                const std::string _str = str.substr(p, memfile_blocks_copy);
                write_noalloc(_str.data(), _str.size(), p);
            }
        }

        void MemoryFile::assign(const char* str, const size_t siz)
        {
            set_size(siz);

            for (size_t p = 0; p < siz;) {
                const size_t curr_siz = (((siz - p) < memfile_blocks_copy) ? (siz - p) : memfile_blocks_copy);
                write_noalloc(str + p, curr_siz, p);
                p += curr_siz;
            }
        }

        void MemoryFile::pop_back()
        {
            if (m_size_now > 1) set_size(m_size_now - 1);
        }

        void MemoryFile::swap(MemoryFile&& oth)
        {
            MemoryFile _temp = std::move(*this);
            *this = std::move(oth);
            oth = std::move(_temp);
        }

        std::string MemoryFile::substr(const size_t beg, const size_t siz) const
        {
            return read_noalloc(siz, beg);
        }

    }
}