#pragma once

class FunctionWrapper {
public:
	typedef void(*func)(void*);
private:
	func m_fcn = nullptr, m_free = nullptr;
	void* m_arg = nullptr;
public:
	FunctionWrapper(const FunctionWrapper&) = delete;
	void operator=(const FunctionWrapper&) = delete;

	FunctionWrapper() = default;
	FunctionWrapper(func, void* = nullptr, func = nullptr);
	~FunctionWrapper();

	FunctionWrapper(FunctionWrapper&&) noexcept;
	void operator=(FunctionWrapper&&) noexcept;

	void set(func, void* = nullptr, func = nullptr);
	void* get_arg();

	bool run() const;
	void free();
};