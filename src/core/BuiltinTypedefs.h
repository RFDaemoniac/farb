#ifndef FARB_BUILTIN_TYPEDEFS_H
#define FARB_BUILTIN_TYPEDEFS_H

#include "../../lib/value_ptr/value_ptr.hpp"
#include <unordered_set>
#include <unordered_map>

namespace Farb
{

// rmf todo: @Implement HString or another pooled string option
// ideally one that allows for a constexpr constructor
// and move it out of BuiltinTypedefs
// we could maybe implement this as a pointer to char array with a length,
// then compile time constants could just be pointed at as normal
// and then registered in the hashmap at run time
// and runtime strings would be stored in memory owned by the hashmap
// but not the hashmap itself
// and comparisons are just about whether the location (and size) are the same
using HString = std::string;
using uint = unsigned int;
using byte = unsigned char;

// not actually a builtin but treated as such
template<typename T>
using value_ptr = smart_ptr::value_ptr<T>;

template<typename T>
using CRef = std::reference_wrapper<const T>;

template<typename T>
using Ref = std::reference_wrapper<T>;

template<typename T>
using Set = std::unordered_set<T>;

template<typename TKey, typename TVal>
using Map = std::unordered_map<TKey, TVal>;

template<typename TKey, typename TVal>
using Table = std::unordered_map<TKey, TVal>;

} // namespace Farb

#endif // FARB_BUILTIN_TYPEDEFS_H
