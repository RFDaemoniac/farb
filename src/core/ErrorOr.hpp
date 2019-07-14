#ifndef FARB_ERROR_OR_HPP
#define FARB_ERROR_OR_HPP

#include <memory>

#include "Error.hpp"

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
	ErrorOr(Error&& error)
		: isError(true)
		, error(std::move(error))
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
		isError = other.isError;
		if (isError)
		{
			error = std::move(other.error);
		}
		else
		{
			value = other.value;
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

#define CHECK_RETURN(functionCall) \
	({ \
		auto result = functionCall; \
		if (result.IsError()) \
		{ \
			return result.GetError(); \
		} \
		result.GetValue(); \
	})

} // namespace Farb

#endif // FARB_ERROR_OR_HPP