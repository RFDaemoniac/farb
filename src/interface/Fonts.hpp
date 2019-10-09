#ifndef FARB_FONTS_HPP
#define FARB_FONTS_HPP

#include "../core/BuiltinTypedefs.h"
#include "../core/NamedType.hpp"

namespace Farb
{

namespace UI
{

NAMED_TYPE_TAG(FontName);
using FontName = NamedType<std::string, FontNameTag>;

//NAMED_TYPE(std::string, FontName);

} // namespace UI

} // namespace Farb

#endif // FARB_FONTS_HPP