#ifndef FARB_BUILTIN_TYPEDEFS_H
#define FARB_BUILTIN_TYPEDEFS_H

namespace Farb
{

// rmf todo: @implement HString or another pooled string option
// ideally one that allows for a constexpr constructor
// and move it out of BuiltinTypedefs
using HString = std::string;
using uint = unsigned int;
using byte = unsigned char;

}

#endif // FARB_BUILTIN_TYPEDEFS_H
