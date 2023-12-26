#pragma once

#include <cstring>

class Exception {
    char* m_desc = nullptr;
public:
    Exception(const char*);
    Exception(const Exception&) = delete;
    Exception(Exception&&) noexcept;
    void operator=(const Exception&) = delete;
    void operator=(Exception&&) noexcept;
    ~Exception();

    char* c_str() const;
};