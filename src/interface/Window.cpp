#include <cmath>

#include "Window.h"

namespace Farb
{

namespace UI
{

Window::Window(int width, int height, std::string name)
	: window(nullptr)
{
	window.reset(tigrWindow(width, height, name.c_str(), TIGR_3X));
}

bool Window::Render(const Node& tree)
{
	Dimensions root{ 0, 0, window->w, window->h };
	auto result = ComputeDimensions(root, root, tree);
	if (result.IsError())
	{
		result.GetError().Log();
		return false;
	}
	Tree<Dimensions> dimensions = result.GetValue();
	auto renderResult = Render(dimensions, tree);
	if (renderResult.IsError())
	{
		renderResult.GetError().Log();
		return false;
	}
	return true;
}

ErrorOr<int> ComputeScalar(int windowSize, int parentSize, const Scalar& scalar)
{
	switch(scalar.units)
	{
	case Units::Pixels:
		return static_cast<int>(round(scalar.amount));
	case Units::PercentOfParent:
		return static_cast<int>(round(scalar.amount * parentSize));
	case Units::PercentOfScreen:
		return static_cast<int>(round(scalar.amount * windowSize));
	default:
		return Error("UI::Node scalar unit is None");
	}
}

ErrorOr<Tree<Dimensions> > Window::ComputeDimensions(
	const Dimensions& window,
	const Dimensions& parent,
	const Node& node)
{
	Tree<Dimensions> dimensionsTree;
	Dimensions& dimensions = dimensionsTree.value;

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

	for (auto & child : node.children)
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

ErrorOr<Success> Window::Render(
	const Tree<Dimensions>& dimensions,
	const Node& node)
{
	if (!node.image.filePath.empty())
	{
		if (dimensions.value.width != node.image.spriteLocation.width
			|| dimensions.value.height != node.image.spriteLocation.height)
		{
			return Error("Can't scale or 9-slice images yet");
		}
		// rmf todo: impelement 9 slicing, scaling, and sprite maps, (and rotation?)
		tigrBlit(
			window.get(),
			node.image.bitmap.get(),
			dimensions.value.x,
			dimensions.value.y,
			node.image.spriteLocation.x,
			node.image.spriteLocation.y,
			dimensions.value.width,
			dimensions.value.height);
	}
	if (!node.text.unparsedText.empty())
	{
		//rmf todo: 
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
	return Success();
}

} // namespace UI


} // namespace Farb