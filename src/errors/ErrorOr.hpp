#ifndef FARB_ERROR_OR_H
#define FARB_ERROR_OR_H

#include <string>
#include <memory>
#include <iostream>

namespace Farb
{

struct Error
{
	std::string message;
	std::shared_ptr<Error> parent;

	Error(std::string message, Error* parent = nullptr)
		: message(message)
		, parent(parent)
	{
	}

	Error(const Error& error)
		: message(error.message)
		, parent(error.parent)
	{
	}

	Error& operator=(const Error& error)
	{
		message = error.message;
		parent = error.parent;
		return *this;
	}

	void Log(uint indentation = 0)
	{
		for(uint i = 0; i < indentation; ++i)
		{
			std::cout << "	";
		}
		std::cout << message << std::endl;
		if (parent != nullptr)
		{
			parent->Log(indentation + 1);
		}
	}
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

} // namespace Farb

#endif // FARB_ERROR_OR_H