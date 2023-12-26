#pragma once

#include <cJSON.h>

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
    bool is_number_big() const; // If it is a number and it's int representation is INT_MAX or INT_MIN, this is true
    bool is_string() const;     // Represents a string value (with \0 terminated)
    bool is_array() const;      // Represents an array value
    bool is_object() const;     // Represents an object value
    bool is_raw() const;        // Represents any kind of JSON that is stored as a zero terminated array of characters

    bool is_root() const;       // If it is set to delete on destroy, this must be the root!

    bool        get_bool() const;
    double      get_double() const;
    int         get_int() const;
    bool        has_object(const char*) const;
    bool        has_object_insensitive(const char*) const;
    pJSON       get_object(const char*) const;
    pJSON       get_object_insensitive(const char*) const;
    const char* get_raw() const;
    const char* get_string() const;
    
    pJSON       operator[](const char*);
    operator bool() const;
    operator double() const;
    operator int() const;
    operator const char*() const;
};
