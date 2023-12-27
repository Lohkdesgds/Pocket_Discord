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

bool pJSON::is_root() const
{
    return m_delete && m_json;
}

bool pJSON::has_object(const char* name) const
{
    return to_object(name).m_json != nullptr;
}

bool pJSON::has_object_insensitive(const char* name) const
{
    return to_object_insensitive(name).m_json != nullptr;
}

bool pJSON::to_bool() const
{
    return m_json ? m_json->type == cJSON_True : false;
}

double pJSON::to_double() const
{
    return m_json ? m_json->valuedouble : 0.0;
}

int64_t pJSON::to_int() const
{
    return m_json ? m_json->valueint : 0;
}

uint64_t pJSON::to_uint64() const
{
    return m_json ? m_json->valueuint : 0;
}

pJSON pJSON::to_object(const char* name) const
{
    return pJSON(m_json ? cJSON_GetObjectItemCaseSensitive(m_json, name) : nullptr);
}

pJSON pJSON::to_object_insensitive(const char* name) const
{
    return pJSON(m_json ? cJSON_GetObjectItem(m_json, name) : nullptr);
}

const char* pJSON::to_raw() const
{
    return m_json ? m_json->valuestring : nullptr;
}

const char* pJSON::to_string() const
{
    return m_json ? m_json->valuestring : nullptr;
}

pJSON pJSON::operator[](const char* s)
{
    return to_object(s);
}