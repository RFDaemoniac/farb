#ifndef FARB_ERROR_OR_H
#define FARB_ERROR_OR_H

#include <string>
#include <memory>

namespace Farb
{

struct Error
{
	std::string message;

	std::unique_ptr<Error> parent;

}

template<typename T>
struct ErrorOr
{
	bool isError;
	Error error;
	T value;

	ErrorOr(&&Error error)
		: isError(true)
		, error(error)
	{ }

	ErrorOr(&&T value)
		: isError(false)
		, value(value)
	{ }

	bool IsError()
	{
		return isError;
	}

	&&T GetValue()
	{
		return value;
	}
}

}

#endif // FARB_ERROR_OR_H