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
	
}

} // namespace Farb

#endif // FARB_OWNED_PTR_HPP