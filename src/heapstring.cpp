#include "heapstring.h"
#include "defaults.h"

#include <string.h>
#include <stdio.h>

char* HeapString::alloc_intern(const size_t l)
{
    char* dat = new char[l + 1];
    dat[l] = '\0';
    return dat;
}

HeapString::HeapString(const char* s, size_t l)
{
    if (s == nullptr) return;
    if (l == 0) l = strlen(s);
    mem = alloc_intern(l);
    memcpy(mem, s, l);
    len = l;
}

HeapString::HeapString(HeapString&& o) noexcept
    : mem(o.mem), len(o.len)
{
    o.mem = nullptr;
    o.len = 0;
}

HeapString::~HeapString()
{
    free();
}

void HeapString::operator=(HeapString&& o) noexcept
{
    free();
    mem = o.mem;
    len = o.len;
}

HeapString& HeapString::operator=(const char* s) noexcept
{
    free();
    const size_t l = strlen(s);
    if (l == 0) return *this;
    mem = alloc_intern(l);
    memcpy(mem, s, l);
    len = l;
    return *this;
}

HeapString& HeapString::operator+=(const char* s) noexcept
{
    if (len == 0) return *this = s;

    const size_t l = strlen(s);
    if (l == 0) return *this;
    char* tar = alloc_intern(l + len);
    memcpy(tar, mem, len);
    delete[] mem;
    mem = tar;
    memcpy(mem + len, s, l);
    len = l + len;
    return *this;
}

void HeapString::set(const char* s, size_t l)
{
    free();
    if (s == nullptr) return;
    if (l == 0) l = strlen(s);
    mem = alloc_intern(l);
    memcpy(mem, s, l);
    len = l;
}

void HeapString::append(const char* s, size_t l)
{
    if (len == 0) {
        set(s, l);
        return;
    }
    if (s == nullptr) return;
    if (l == 0) l = strlen(s);
    
    char* tar = alloc_intern(l + len);
    memcpy(tar, mem, len);
    delete[] mem;
    mem = tar;
    memcpy(mem + len, s, l);
    len = l + len;
}

void HeapString::append(const char v)
{
    append((const char*)&v, 1);
}

bool HeapString::append(const uint16_t v)
{
    int l = snprintf(nullptr, 0, "%u", v);
    if (l < 0) return false;
    char* tar = alloc_intern(l + len);
    memcpy(tar, mem, len);
    delete[] mem;
    mem = tar;
    snprintf(mem + len, l + 1, "%u", v);
    len = l + len;
    return true;
}

bool HeapString::append(const uint32_t v)
{
    int l = snprintf(nullptr, 0, "%lu", v);
    if (l < 0) return false;
    char* tar = alloc_intern(l + len);
    memcpy(tar, mem, len);
    delete[] mem;
    mem = tar;
    snprintf(mem + len, l + 1, "%lu", v);
    len = l + len;
    return true;
}

bool HeapString::append(const uint64_t v)
{
    int l = snprintf(nullptr, 0, "%llu", v);
    if (l < 0) return false;
    char* tar = alloc_intern(l + len);
    memcpy(tar, mem, len);
    delete[] mem;
    mem = tar;
    snprintf(mem + len, l + 1, "%llu", v);
    len = l + len;
    return true;
}

bool HeapString::append(const int16_t v)
{
    int l = snprintf(nullptr, 0, "%i", v);
    if (l < 0) return false;
    char* tar = alloc_intern(l + len);
    memcpy(tar, mem, len);
    delete[] mem;
    mem = tar;
    snprintf(mem + len, l + 1, "%i", v);
    len = l + len;
    return true;
}

bool HeapString::append(const int32_t v)
{
    int l = snprintf(nullptr, 0, "%li", v);
    if (l < 0) return false;
    char* tar = alloc_intern(l + len);
    memcpy(tar, mem, len);
    delete[] mem;
    mem = tar;
    snprintf(mem + len, l + 1, "%li", v);
    len = l + len;
    return true;
}

bool HeapString::append(const int64_t v)
{
    int l = snprintf(nullptr, 0, "%lli", v);
    if (l < 0) return false;
    char* tar = alloc_intern(l + len);
    memcpy(tar, mem, len);
    delete[] mem;
    mem = tar;
    snprintf(mem + len, l + 1, "%lli", v);
    len = l + len;
    return true;
}

bool HeapString::append(const float v)
{
    int l = snprintf(nullptr, 0, "%f", v);
    if (l < 0) return false;
    char* tar = alloc_intern(l + len);
    memcpy(tar, mem, len);
    delete[] mem;
    mem = tar;
    snprintf(mem + len, l + 1, "%f", v);
    len = l + len;
    return true;
}

bool HeapString::append(const double v)
{
    int l = snprintf(nullptr, 0, "%lf", v);
    if (l < 0) return false;
    char* tar = alloc_intern(l + len);
    memcpy(tar, mem, len);
    delete[] mem;
    mem = tar;
    snprintf(mem + len, l + 1, "%lf", v);
    len = l + len;
    return true;
}


size_t HeapString::size() const
{
    return len;
}

void HeapString::free()
{
    DEL_EM(mem);
    len = 0;
}

const char* HeapString::c_str() const
{
    return mem;
}

void HeapString::prepare(const size_t l)
{
    free();
    alloc_intern(l);
}

char* HeapString::prepared_data()
{
    return mem;
}
