#ifndef FARB_ERROR_HPP
#define FARB_ERROR_HPP

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

} // namespace Farb

#endif // FARB_ERROR_HPP