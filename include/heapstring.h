#pragma once

#include <cstddef>

class HeapString {
    char* mem = nullptr;
    size_t len = 0;
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

    size_t size() const;
    void free();

    const char* c_str() const;

};