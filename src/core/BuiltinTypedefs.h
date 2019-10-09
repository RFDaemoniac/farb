#ifndef FARB_BUILTIN_TYPEDEFS_H
#define FARB_BUILTIN_TYPEDEFS_H

#include "../../lib/valuable/value_ptr.hpp"

namespace Farb
{

// rmf todo: @implement HString or another pooled string option
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
using value_ptr = valuable::value_ptr<T>;

template<typename T>
using CRef = std::reference_wrapper<const T>;

template<typename T>
using Ref = std::reference_wrapper<T>;

} // namespace Farb

#endif // FARB_BUILTIN_TYPEDEFS_H
