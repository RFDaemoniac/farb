#ifndef FARB_CONTAINER_EXTENSIONS_HPP
#define FARB_CONTAINER_EXTENSIONS_HPP

#include <unordered_set>

namespace Farb
{

template<typename T>
inline bool Disjoint(std::unordered_set<T> a, std::unordered_set<T> b)
{
	if (a.size() < b.size())
	{
		for (const T & value : a)
		{
			if (b.contains(value))
			{
				return false;
			}
		}
	}
	else
	{
		for (const T & value : b)
		{
			if (a.contains(value))
			{
				return false;
			}
		}
	}
	return true;
}

template<typename T>
inline bool Intersecting(std::unordered_set<T> a, std::unordered_set<T> b)
{
	return !Disjoint(a, b);
}

} // namespace Farb

#endif // FARB_CONTAINER_EXTENSIONS_HPP