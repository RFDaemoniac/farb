#include "Window.h"

namespace Farb
{

namespace UI
{

bool Window::Render(const Node& tree)
{
	ComputedDimensions dimensions{ 0, 0, window->w, window->h };
	auto result = Render(dimensions, dimensions, tree);
	if (result.IsError())
	{
		result.GetError().Log();
		return false;
	}
	return true;
}

ErrorOr<int> ComputeScalar(int windowSize, int parentSize, const Scalar& scalar)
{
	switch(scalar.type)
	{
	case Units::Pixels:
		return static_cast<int>(round(node.width.scalar.amount));
	case Units::PercentOfParent:
		return static_cast<int>(round(scalar.amount * parentSize));
	case Units::PercentOfScreen:
		return static_cast<int>(round(scalar.amount * windowSize));
	default:
		return Error("UI::Node scalar unit is None");
	}
}

ErrorOr<ComputedDimensions> ComputeDimensions(ComputedDimensions& window, ComputedDimensions& parent, const Node& node)
{
	ComputedDimensions dimensions;

	bool 

	switch(node.width.type)
	{
	case SizeType::Scalar:
		dimensions.width = CHECK_RETURN(ComputeScalar(window.width, parent.width, node.width.scalar));
		break;
	default:
		return Error("UI::Node width sizetype other than scalar");
	}

	switch(node.height.type)
	{
	case SizeType::Scalar:
		dimensions.height = CHECK_RETURN(ComputeScalar(window.height, parent.height, node.height.scalar));
	default:
		return Error("UI::Node height sizetype other than scalar");
	}

	foreach (auto & child : children)
	{
		auto result = Render(window, dimensions, child);
		if (result.IsError())
		{
			return result.GetError();
		}
	}

	return dimensions;
}

} // namespace UI


} // namespace Farb