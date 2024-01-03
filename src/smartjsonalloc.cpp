#include "smartjsonalloc.h"

constexpr size_t max_mem_len = 512; // bytes, testing fixed for now

uint16_t MixedJSONRef::m_fp_name_ref = 0;

MixedJSONRef::MixedJSONRef(const size_t exp)
    : IterateableJSONRef(), m_mem(new char[(exp > max_mem_len ? max_mem_len : exp)]),
      m_mem_len(exp > max_mem_len ? max_mem_len : exp), m_total_len(exp), m_fp()
{
    if (exp > max_mem_len) {
        ESP_LOGI("MX", "Had to alloc file for MixedJSONRef this time. Size extra: %zu", exp - max_mem_len);
        char nameing[32];
        sprintf(nameing, "swap_%d.mem", (int)m_fp_name_ref++);
        m_fp = File(nameing, "wb+");
        if (m_fp.bad()) throw std::runtime_error("Could not create key file for MixedJSONRef");
        m_fp.seek(exp - max_mem_len - 1, File::seek_mode::SET);
        m_fp.putc('\0');
        m_fp.putc('\0');
    }
    else {
        ESP_LOGI("MX", "Allocated %zu in RAM for JSON at %p", m_mem_len, m_mem);
    }
    memset(m_mem, '\0', m_mem_len);
}

MixedJSONRef::~MixedJSONRef()
{
    m_fp.close();
    if (m_mem) {
        ESP_LOGI("MX", "Freeing up %zu in RAM from at %p", m_mem_len, m_mem);
        delete[] m_mem;
        m_mem = nullptr;
    }
}

char MixedJSONRef::get(const size_t at) const
{
    if (at < m_mem_len) return m_mem[at];
    if (at >= max_mem_len) return '\0';
    if (m_fp.bad()) return '\0';
    m_fp.seek(at - m_mem_len, File::seek_mode::SET);
    int v = m_fp.getc();
    return v < 0 ? '\0' : static_cast<char>(v);
}

void MixedJSONRef::read(char* ptr, const size_t len, const size_t at) const
{
        // m_mem_len = 100, at = 80, len = 30
    if (at < m_mem_len) memcpy(ptr, m_mem + at, m_mem_len - at < len ? m_mem_len - at : len);
    if (at + len > m_mem_len && m_fp.good()) {
        m_fp.seek(at < m_mem_len ? 0 : at - m_mem_len, File::seek_mode::SET);
        m_fp.read(ptr + (at < m_mem_len ? m_mem_len - at : 0), len - (at < m_mem_len ? m_mem_len - at : 0));
    }
//    if (at + len <= m_mem_len) {
//        memcpy(ptr, m_mem + at, len);
//        return;
//    }
//    if (m_fp.bad()) {
//        return;
//    }
//    if (at > m_mem_len) {
//        m_fp.seek(at - m_mem_len, File::seek_mode::SET);
//        m_fp.read(ptr, len);
//        return;
//    }
//
//    memcpy(ptr, m_mem + at, m_mem_len - at);
//    m_fp.seek(0, File::seek_mode::SET);
//    m_fp.read(ptr + (m_mem_len - at), len - (m_mem_len - at));
}

void MixedJSONRef::put(const char& what, const size_t at)
{
    if (at < m_mem_len) m_mem[at] = what;
    else {
        if (m_fp.bad()) return;
        m_fp.seek(at - m_mem_len, File::seek_mode::SET);
        m_fp.write(&what, 1);
    }
}
void MixedJSONRef::write(const char* ptr, const size_t len, const size_t at)
{
//    for(size_t p = 0; p < len; ++p) put(ptr[p], at + p);
    if (at < m_mem_len) memcpy(m_mem + at, ptr, m_mem_len - at < len ? m_mem_len - at : len);
    if (at + len > m_mem_len && m_fp.good()) {
        m_fp.seek(at < m_mem_len ? 0 : at - m_mem_len, File::seek_mode::SET);
        m_fp.write(ptr + (at < m_mem_len ? m_mem_len - at : 0), len - (at < m_mem_len ? m_mem_len - at : 0));
    }


//    if (at + len <= m_mem_len) {
//        memcpy(m_mem + at, ptr, len);
//        return;
//    }
//    if (m_fp.bad()) return;
//
//    if (at > m_mem_len) {
//        m_fp.seek(at - m_mem_len, File::seek_mode::SET);
//        m_fp.write(ptr, len);
//        return;
//    }
//
//    memcpy(m_mem + at, ptr, m_mem_len - at);
//    m_fp.seek(0, File::seek_mode::SET);
//    m_fp.write(ptr + (m_mem_len - at), len - (m_mem_len - at));
}

size_t MixedJSONRef::max_off() const
{
    return m_total_len;
}