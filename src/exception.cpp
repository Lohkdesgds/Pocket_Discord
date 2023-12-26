#include "exception.h"

static const char* exc_corrupted_err = "CORRUPTED ERROR";

Exception::Exception(const char* s)
{
    if (!s) m_desc = (char*)exc_corrupted_err;
    else {
        const size_t l = strlen(s);
        m_desc = new char[l + 1];
        memcpy(m_desc, s, l + 1);
    }
}

Exception::Exception(Exception&& e) noexcept
    : m_desc(e.m_desc)
{
    e.m_desc = nullptr;
}

void Exception::operator=(Exception&& e) noexcept
{
    if (m_desc) delete[] m_desc;
    m_desc = e.m_desc;
    e.m_desc = nullptr;
}

Exception::~Exception()
{
    if (m_desc) delete[] m_desc;
    m_desc = nullptr;
}

char* Exception::c_str() const
{
    return m_desc;
}
