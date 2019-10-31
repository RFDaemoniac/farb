#ifndef FARB_ERROR_OR_HPP
#define FARB_ERROR_OR_HPP

#include <memory>
#include <type_traits>

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

// SFINAE type inspection
template<typename TVal>
struct IsErrorOr : std::false_type { };

template<typename TVal>
struct IsErrorOr<ErrorOr<TVal> > : std::true_type { };

template<typename T>
struct UnwrapErrorOr
{
	using TVal = T;
};

template<typename T>
struct UnwrapErrorOr<ErrorOr<T> >
{
	using TVal = T;
};

template<typename T>
T ValueOrDefault(ErrorOr<T> result, T default_value)
{
	if (result.IsError())
	{
		return default_value;
	}
	return result.GetValue();
}

} // namespace Farb

#endif // FARB_ERROR_OR_HPP