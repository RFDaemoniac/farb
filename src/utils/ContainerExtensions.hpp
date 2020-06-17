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

template<typename T>
inline bool SuperSet(std::unordered_set<T> a, std::unordered_set<T> b)
{
	if (a.size() < b.size())
	{
		return false;
	}
	for (auto && value : b)
	{
		if (!a.contains(value))
		{
			return false;
		}
	}
	return true;
}

template<typename TContainer, typename TType>
inline bool Contains(const TContainer & container, const TType & value)
{
	if constexpr(std::is_same<TContainer, std::vector<TType>>::value)
	{
		return std::find(container.begin(), container.end(), value) != container.end();
	}
	else
	{
		return container.count(value) > 0;
	}
}

} // namespace Farb

#endif // FARB_CONTAINER_EXTENSIONS_HPP