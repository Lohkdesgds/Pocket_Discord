#pragma once

#include <cstddef>

class HeapString {
    char* mem = nullptr;
    size_t len = 0;

    char* alloc_intern(const size_t);
public:
    HeapString(const char*, size_t = 0);
    HeapString(const HeapString&) = delete;
    HeapString(HeapString&&) noexcept;
    HeapString() = default;
    ~HeapString();

    void operator=(const HeapString&) = delete;
    void operator=(HeapString&&) noexcept;

    HeapString& operator=(const char*) noexcept;
    HeapString& operator+=(const char*) noexcept;

    void set(const char*, size_t);
    void append(const char*, size_t);

    size_t size() const;
    void free();

    const char* c_str() const;
};