#pragma once

#include "../deps/cJSONL/cJSON.h"

class pJSON {
    cJSON* m_json;
    const bool m_delete;

    pJSON(cJSON*);
    
public:
    pJSON(pJSON&&) noexcept;
    void operator=(pJSON&&) noexcept;
    pJSON(const char*, const size_t);

    pJSON(const pJSON&) = delete;
    void operator=(const pJSON&) = delete;

    ~pJSON();

    bool is_invalid() const;    // Represents an invalid item that doesn't contain any value. You automatically have this type if you set the item to all zero bytes.
    bool is_bool() const;       // Represents a boolean value
    bool is_null() const;       // Represents a null value
    bool is_number() const;     // Represents a number value
    bool is_string() const;     // Represents a string value (with \0 terminated)
    bool is_array() const;      // Represents an array value
    bool is_object() const;     // Represents an object value
    bool is_raw() const;        // Represents any kind of JSON that is stored as a zero terminated array of characters

    bool is_root() const;       // If it is set to delete on destroy, this must be the root!

    bool        has_object(const char*) const;
    bool        has_object_insensitive(const char*) const;

    bool        to_bool() const;
    double      to_double() const;
    int64_t     to_int() const;
    uint64_t    to_uint64() const;
    pJSON       to_object(const char*) const;
    pJSON       to_object_insensitive(const char*) const;
    const char* to_raw() const;
    const char* to_string() const;
    
    pJSON       operator[](const char*);
};
