#include "heapstring.h"
#include "defaults.h"

#include <string.h>

HeapString::HeapString(const char* s, size_t l)
{
    if (s == nullptr) return;
    if (l == 0) l = strlen(s);
    mem = new char[l];
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
    mem = new char[l];
    len = l;
    return *this;
}

HeapString& HeapString::operator+=(const char* s) noexcept
{
    if (len == 0) return *this = s;

    const size_t l = strlen(s);
    if (l == 0) return *this;
    char* tar = new char[l + len];
    memcpy(tar, mem, len);
    delete[] mem;
    mem = tar;
    memcpy(mem + len, s, l);
    len = l + len;
    return *this;
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