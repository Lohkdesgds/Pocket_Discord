#pragma once

#include "LJSON/json.h"
#include "filehandler.h"

class MixedJSONRef : public Lunaris::IterateableJSONRef {
    char* m_mem = nullptr;
    const size_t m_mem_len = 0;
    size_t m_total_len = 0;
    mutable File m_fp;
    static uint16_t m_fp_name_ref;
public:
    MixedJSONRef(const size_t len);
    ~MixedJSONRef();

    char get(const size_t) const;
    void read(char*, const size_t, const size_t) const;
    void put(const char&, const size_t);
    void write(const char*, const size_t, const size_t);
    size_t max_off() const;
};