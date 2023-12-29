#include "heapstring.h"
#include "defaults.h"

#include <string.h>

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
    if (l == 0) return;
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
    if (l == 0) return;
    char* tar = alloc_intern(l + len);
    memcpy(tar, mem, len);
    delete[] mem;
    mem = tar;
    memcpy(mem + len, s, l);
    len = l + len;
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