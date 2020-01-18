#ifndef FARB_OWNED_PTR_HPP
#define FARB_OWNED_PTR_HPP

#include <string>
#include <memory>
#include <iostream>

namespace Farb
{

template<typename T>
struct OwnedPtr
{
	T * value;

	OwnedPtr(T * value)
		: value(value)
	{ }

	OwnedPtr(OwnedPtr&& other)
		: value(other.value)
	{
		other.value = nullptr;
	}

	~OwnedPtr()
	{
		if (value != nullptr)
		{
			delete value;
			value = nullptr;
		}
	}
}

} // namespace Farb

#endif // FARB_OWNED_PTR_HPP