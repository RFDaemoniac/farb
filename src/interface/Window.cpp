#include "Window.h"

namespace Farb
{

namespace UI
{

struct ComputedDimensions
{
	int x, y;
	int width, height;
}

bool Window::Render(const Node& tree)
{
	ComputedDimensions root{ 0, 0, window->w, window->h };
	auto result = ComputeDimensions(root, root, tree);
	if (result.IsError())
	{
		result.GetError().Log();
		return false;
	}
	Tree<ComputedDimensions> dimensions = result.GetValue();
	Render(dimensions, tree);
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
	Tree<ComputeDimensions> dimensionsTree;
	ComputedDimensions& dimensions = dimensions.value;

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
		auto result = ComputeDimensions(window, dimensions, child);
		if (result.IsError())
		{
			return result.GetError();
		}
		dimensionsTree.children.push_back(result.GetValue());
	}
	return dimensionsTree;
}

ErrorOr<void> Window::Render(
	const Tree<ComputedDimensions>& dimensions,
	const Node& node)
{
	//rmf todo: actually render contents
	if (!node.image.filePath.empty())
	{
		
	}
	if (!node.text.unparsedText.empty())
	{

	}
	if (dimensions.children.size() != node.children.size())
	{
		return Error("Dimensions have different shape then ui node tree");
	}
	for (int i = 0; i < dimensions.children.size(); ++i)
	{
		auto result = Render(dimensions.children[i], node.children[i]);
		if (result.IsError())
		{
			return result.GetError();
		}
	}
	return;
}

} // namespace UI


} // namespace Farb