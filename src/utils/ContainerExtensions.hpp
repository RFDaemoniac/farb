#ifndef FARB_CONTAINER_EXTENSIONS_HPP
#define FARB_CONTAINER_EXTENSIONS_HPP

#include <tuple>
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


// https://stackoverflow.com/questions/7110301/generic-hash-for-tuples-in-unordered-map-unordered-set
// function has to live in the std namespace 
// so that it is picked up by argument-dependent name lookup (ADL).
// this is against the standards, but it works
namespace std{
	namespace
	{

		// Code from boost
		// Reciprocal of the golden ratio helps spread entropy
		//     and handles duplicates.
		// See Mike Seymour in magic-numbers-in-boosthash-combine:
		//     https://stackoverflow.com/questions/4948780

		template <class T>
		inline void hash_combine(std::size_t& seed, T const& v) noexcept
		{
			seed ^= hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
		}

		// Recursive template code derived from Matthieu M.
		template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
		struct HashValueImpl
		{
		  static void apply(size_t& seed, Tuple const& tuple) noexcept
		  {
			HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
			hash_combine(seed, get<Index>(tuple));
		  }
		};

		template <class Tuple>
		struct HashValueImpl<Tuple,0>
		{
		  static void apply(size_t& seed, Tuple const& tuple) noexcept
		  {
			hash_combine(seed, get<0>(tuple));
		  }
		};
	}

	template <typename ... TT>
	struct hash<std::tuple<TT...>> 
	{
		size_t
		operator()(std::tuple<TT...> const& tt) const noexcept
		{
			size_t seed = 0;
			HashValueImpl<std::tuple<TT...> >::apply(seed, tt);    
			return seed;
		}

	};
}

#endif // FARB_CONTAINER_EXTENSIONS_HPP