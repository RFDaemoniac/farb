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
	// ############# important
	// rmf note: important remember that dimensions are with respect to the window
	// not with respect to the parent.
	// or rather, you need to pick one or the other.
	// I guess with respect to the parent would be fine, we just need to remember that
	// as we are rendering
	Tree<Dimensions> dimensionsTree;
	Dimensions& dimensions = dimensionsTree.value;

	std::set<DimensionAttributes> computedAttributes;

	bool gotWidth = false;
	bool gotHeight = false;

	if (node.spec & NodeSpec::Left)
	{
		dimensions.x = CHECK_RETURN(ComputeScalar(
			window.width, parent.width, node.left));

		if (node.spec & NodeSpec::Right)
		{
			int x2 = parent.width - CHECK_RETURN(ComputeScalar(
				window.width, parent.width, node.right));
			dimensions.width = x2 - dimensions.x;
			gotWidth = true;
		}
	}
	if (node.spec & NodeSpec::Top)
	{
		dimensions.y = ComputeScalar(window.height, parent.height, node.top);

		if (node.spec & NodeSpec::Bottom)
		{
			int y2 = parent.height - CHECK_RETURN(ComputeScalar(
				window.height, parent.height, node.bottom));
			dimensions.height = y2 - dimensions.y;
			gotHeight = true;
		}
	}

	if (!gotWidth && node.width.type == SizeType::Scalar)
	{
		dimensions.width = CHECK_RETURN(ComputeScalar(
			window.width, parent.width, node.width.scalar));
		gotWidth = true;
	}
	if (!gotHeight && node.height.type == SizeType::Scalar)
	{
		dimensions.height = CHECK_RETURN(ComputeScalar(
			window.height, parent.height, node.width.height));
		gotHeight = true;
	}
	if ((!gotWidth && node.width.type == SizeType::FitContents
			|| !gotHeight && node.height.type == SizeType::FitContents)
		&& !node.image.filePath.empty())
	{
		if (!gotHeight
			&& !gotWidth
			&& node.width.type == SizeType::FitContents
			&& node.height.type == SizeType::FitContents
			&& !node.image.filePath.empty())
		{
			dimensions.width = node.image.spriteLocation.width;
			gotWidth = true;
			dimensions.height = node.image.spriteLocation.height;
			gotHeight = true;
		}
	}
	

	{
		switch(node.width.type)
		{
		case SizeType::Scalar:
			break;
		case SizeType::FitContents:
			if (!gotHeight) { return Error("UI::Node width fit contents"); }
		default:
			return Error("UI::Node width sizetype other than scalar");
		}
	}

	switch(node.width.type)
	{
	case SizeType::Scalar:
		
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

	// -1 is used as a sentinal value for parents dependent on child size
	if (!gotWidth && node.width.type == SizeType::FitChildren)
	{
		dimensions.width = -1;
	}
	if (!gotHeight && node.height.type == SizeType::FitChildren)
	{
		dimensions.height = -1;
	}

	for (auto & child : node.children)
	{
		auto childTree = CHECK_RETURN(ComputeDimensions(
			window, dimensions, child));
		dimensionsTree.children.push_back(childTree);
	}

	if (!gotWidth && node.width.type == SizeType::FitChildren)
	{
		// get max width + x of all children
	}
	if (!gotHeight && node.height.type == SizeType::FitChildren)
	{
		// get max height + y of all children
		// padding? could assume it's the same as the min y of all children
	}

	if ((parent.height > 0 && dimensions.height + dimensions.y > parent.height)
		|| (parent.width > 0 && dimensions.width + dimensions.x > parent.width)
		|| dimensions.x < 0
		|| dimensions.y < 0
		|| dimensions.width < 0
		|| dimensions.height < 0)
	{
		// rmf todo: clipping for scrollers, parents dependent on child size
		// there are several exceptions to this rule...
		return Error("Child breaks bounds of parent");
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