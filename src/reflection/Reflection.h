// based on http://donw.io/post/reflection-cpp-1/
#ifndef FARB_REFLECTION_H
#define FARB_REFLECTION_H

#include "ReflectionTypes.h"

namespace Farb
{

namespace Reflection
{

void SaveObject(Object* object);

void SaveObject(char* data, Type* type);

void SaveBuiltin(char* data, Type* type);

}


} // namespace Reflection

} // namespace Farb

#endif // FARB_REFLECTION_H