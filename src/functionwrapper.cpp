#include "functionwrapper.h"
#include "defaults.h"

FunctionWrapper::FunctionWrapper(FunctionWrapper::func f, void* arg, FunctionWrapper::func free_arg)
	: m_fcn(f), m_free(free_arg), m_arg(arg)
{}

FunctionWrapper::~FunctionWrapper()
{
	free();
}

FunctionWrapper::FunctionWrapper(FunctionWrapper&& o) noexcept
	: m_fcn(o.m_fcn), m_free(o.m_free), m_arg(o.m_arg)
{
	o.m_fcn = nullptr;
	o.m_free = nullptr;
	o.m_arg = nullptr;
}

void FunctionWrapper::operator=(FunctionWrapper&& o) noexcept
{
	free();
	EXC_NULL(m_fcn, o.m_fcn);
	EXC_NULL(m_free, o.m_free);
	EXC_NULL(m_arg, o.m_arg);
}

void FunctionWrapper::set(FunctionWrapper::func f, void* arg, FunctionWrapper::func free_arg)
{
	free();
	m_fcn = f;
	m_arg = arg;
	m_free = free_arg;
}

void* FunctionWrapper::get_arg()
{
	return m_arg;
}

bool FunctionWrapper::run() const
{
	try {
		if (m_fcn) m_fcn(m_arg);
		return true;
	}
	catch (...) {
		return false;
	}
}

void FunctionWrapper::free()
{
	if (m_arg) {
		if (m_free) m_free(m_arg);
	}
	m_arg = nullptr;
	m_free = nullptr;
	m_fcn = nullptr;
}