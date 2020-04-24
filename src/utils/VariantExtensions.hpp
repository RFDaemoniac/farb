#ifndef FARB_VARIANT_EXTENSIONS_HPP
#define FARB_VARIANT_EXTENSIONS_HPP

#include <variant>

namespace Farb
{

template <typename TFrom, typename TTo>
TTo VariantCast(TFrom&& v)
{
	return std::visit(
		[](auto&& arg) -> TTo
		{
			return arg ;
		},
		v);
}

} // namespace Farb

#endif // FARB_VARIANT_EXTENSIONS_HPP