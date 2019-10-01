#ifndef FARB_BUILTIN_TYPEDEFS_H
#define FARB_BUILTIN_TYPEDEFS_H

#include "../../lib/valuable/value_ptr.hpp"

namespace Farb
{

// rmf todo: @implement HString or another pooled string option
// ideally one that allows for a constexpr constructor
// and move it out of BuiltinTypedefs
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
