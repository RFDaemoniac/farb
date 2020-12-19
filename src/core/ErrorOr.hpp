#ifndef FARB_ERROR_OR_HPP
#define FARB_ERROR_OR_HPP

#include <memory>
#include <type_traits>

#include "../../src/core/Error.hpp"

namespace Farb
{

// just used for ErrorOr<Success>
struct Success
{
};

template<typename T>
struct ErrorOr
{
private:
	bool isError;

	union {
		Error error;
		T value;
	};

public:
	ErrorOr(const Error& error)
		: isError(true)
		, error(error)
	{ }

	ErrorOr(Error&& error)
		: isError(true)
		, error(std::move(error))
	{ }

	ErrorOr(const T& value)
		: isError(false)
		, value(value)
	{ }

	ErrorOr(T&& value)
		: isError(false)
		, value(value)
	{ }

	ErrorOr(ErrorOr&& other)
		: isError(other.isError)
	{
		if (isError)
		{
			error = std::move(other.error);
		}
		else
		{
			value = std::move(other.value);
		}
	}

	~ErrorOr()
	{
		if (isError)
		{
			(&error)->Error::~Error();
		}
		else
		{
			(&value)->T::~T();
		}
	}

	ErrorOr& operator=(ErrorOr&& other)
	{
		if (this == &other)
		{
			return *this;
		}
		if (isError != other.isError)
		{
			if (isError)
			{
				(&error)->Error::~Error();
			}
			else
			{
				(&value)->T::~T();
			}
			isError = other.isError;
		}
		if (isError)
		{
			// we are calling an in place new
			// so that we don't try to reassign a garbage error.parent pointer
			// causing a free on random memory
			new (&error) Error(other.error);
		}
		else
		{
			new (&value) T(other.value);
		}
		return *this;
	}

	bool IsError()
	{
		return isError;
	}

	T& GetValue()
	{
		assert(!isError);
		return value;
	}

	Error& GetError()
	{
		assert(isError);
		return error;
	}
};

// this is a gcc extension that is also present in clang but not msvc
// it allows for multiple statements in a single expression
// and evaluates to the last one
#define CHECK_RETURN(functionCall) \
	({ \
		auto erroror_internal_result = functionCall; \
		if (erroror_internal_result.IsError()) \
		{ \
			return erroror_internal_result.GetError(); \
		} \
		erroror_internal_result.GetValue(); \
	})

} // namespace Farb

#endif // FARB_ERROR_OR_HPP
