#pragma once

#include <cstddef>
#include <stdint.h>

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

    void set(const char*, size_t = 0);
    void append(const char*, size_t = 0);

    void append(const char);
    bool append(const uint16_t);
    bool append(const uint32_t);
    bool append(const uint64_t);
    bool append(const int16_t);
    bool append(const int32_t);
    bool append(const int64_t);
    bool append(const float);
    bool append(const double);

    size_t size() const;
    void free();

    const char* c_str() const;

    void prepare(const size_t);
    char* prepared_data();
};