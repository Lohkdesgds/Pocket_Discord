#include "jsoninterface.h"
#include "defaults.h"
#include <limits.h>

pJSON::pJSON(cJSON* ptr)
    : m_json(ptr), m_delete(false)
{
}

pJSON::pJSON(const char* s, const size_t l)
    : m_json(cJSON_ParseWithLength(s, l)), m_delete(true)
{
}

pJSON::pJSON(pJSON&& j) noexcept
    : m_json(j.m_json), m_delete(j.m_delete)
{
    j.m_json = nullptr;
}

void pJSON::operator=(pJSON&& j) noexcept
{
    EXC_NULL(m_json, j.m_json);
}

pJSON::~pJSON()
{
    if (m_json && m_delete) cJSON_Delete(m_json);
    m_json = nullptr;
}

bool pJSON::is_invalid() const
{
    return m_json ? cJSON_IsInvalid(m_json) : false;
}

bool pJSON::is_bool() const
{
    return m_json ? cJSON_IsBool(m_json) : false;
}

bool pJSON::is_null() const
{
    return m_json ? cJSON_IsNull(m_json) : false;
}

bool pJSON::is_number() const
{
    return m_json ? cJSON_IsNumber(m_json) : false;
}

bool pJSON::is_number_big() const
{
    return m_json ? (cJSON_IsNumber(m_json) ? (m_json->valueint == INT_MAX || m_json->valueint == INT_MIN) : false) : false;
}

bool pJSON::is_string() const
{
    return m_json ? (cJSON_IsString(m_json) && m_json->valuestring != nullptr) : false;
}

bool pJSON::is_array() const
{
    return m_json ? cJSON_IsArray(m_json) : false;
}

bool pJSON::is_object() const
{
    return m_json ? cJSON_IsObject(m_json) : false;
}

bool pJSON::is_raw() const
{
    return m_json ? cJSON_IsRaw(m_json) : false;
}

bool pJSON::get_bool() const
{
    return m_json ? m_json->type == cJSON_True : false;
}

double pJSON::get_double() const
{
    return m_json ? m_json->valuedouble : 0.0;
}

int pJSON::get_int() const
{
    return m_json ? m_json->valueint : 0;
}

bool pJSON::has_object(const char* name) const
{
    return get_object(name).m_json != nullptr;
}

bool pJSON::has_object_insensitive(const char* name) const
{
    return get_object_insensitive(name).m_json != nullptr;
}

pJSON pJSON::get_object(const char* name) const
{
    return pJSON(m_json ? cJSON_GetObjectItemCaseSensitive(m_json, name) : nullptr);
}

pJSON pJSON::get_object_insensitive(const char* name) const
{
    return pJSON(m_json ? cJSON_GetObjectItem(m_json, name) : nullptr);
}

const char* pJSON::get_raw() const
{
    return m_json ? m_json->valuestring : nullptr;
}

const char* pJSON::get_string() const
{
    return m_json ? m_json->valuestring : nullptr;
}

pJSON pJSON::operator[](const char* s)
{
    return get_object(s);
}

pJSON::operator bool() const
{
    return is_bool() ? get_bool() : false;
}

pJSON::operator double() const
{
    return is_number() ? get_double() : 0.0;
}

pJSON::operator int() const
{
    return is_number() ? get_int() : 0;
}

pJSON::operator const char*() const
{
    return (is_string() || is_raw()) ? get_string() : nullptr;
}
