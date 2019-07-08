
#include "UINode.h"

namespace Farb
{

namespace UI
{

bool Scalar::ParseAssign(const std::string& value)
{
	amount = 0;
	units = Units::Pixels;
	Error("Scalar::ParseAssign not implemented").Log();
	return false;
}

} // namespace UI

} // namespace Farb