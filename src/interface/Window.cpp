#include <algorithm>
#include <cmath>
#include <limits>
#include <set>

#include "Window.h"
#include "ReflectionDeclare.h"
#include "ReflectionContainers.hpp" // for ToString(Tree<Dimensions>)

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
	tigrClear(window.get(), tigrRGB(0,0,0));
	Dimensions root{ 0, 0, window->w, window->h };
	auto result = ComputeDimensions(root, root, tree);
	if (result.IsError())
	{
		result.GetError().Log();
		return false;
	}
	Tree<Dimensions> dimensions = result.GetValue();
	auto renderResult = Render(0, 0, dimensions, tree);
	if (renderResult.IsError())
	{
		renderResult.GetError().Log();
		std::cout << Reflection::ToString(dimensions) << std::endl;
		return false;
	}
	tigrUpdate(window.get());
	return true;
}

ErrorOr<int> ComputeScalar(int windowSize, int parentSize, const Scalar& scalar)
{
	switch(scalar.units)
	{
	case Units::Pixels:
		return static_cast<int>(round(scalar.amount));
	case Units::PercentOfParent:
		return static_cast<int>(round(scalar.amount * parentSize / 100.f));
	case Units::PercentOfScreen:
		return static_cast<int>(round(scalar.amount * windowSize / 100.f));
	case Units::None:
		return Error("UI::Node scalar unit is None");
	}
}

// Dimensions x and y are relative to the parent x and y
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
	auto& spec = node.spec;

	std::set<DimensionAttribute> computedAttributes;

	auto computeWidthFitContentsImage = [&]() -> ErrorOr<Success>
	{
		int maxWidth = std::numeric_limits<int>::max();
		// rmf todo: could pass in explicitly whether parent width had been defined
		// this implies FitContents also fits parent, which I think is correct
		// since all (except mask, which is not yet implemented) children must fit their parents
		if (parent.width > 0)
		{
			int padding = dimensions.x;
			if (spec & NodeSpec::Right)
			{
				padding += CHECK_RETURN(ComputeScalar(
					window.width, parent.width, node.right));
			}
			maxWidth = std::min(maxWidth, parent.width - padding);
		}
		if (computedAttributes.count(DimensionAttribute::Height))
		{
			// rmf todo: 9sliced images (don't need to maintain ratio)
			float imageRatio =
				static_cast<float>(node.image.spriteLocation.width)
				/ static_cast<float>(node.image.spriteLocation.height);
			dimensions.width = static_cast<int>(round(dimensions.height * imageRatio));
			if (dimensions.width > maxWidth)
			{
				dimensions.width = maxWidth;
				// we should probably reduce height here, too?
				// but what if Y has already been computed?
				// figure it out later
				return Error("Could not maintain aspect ratio of image " + node.image.filePath);
			}
		}
		else
		{
			dimensions.width = std::min(maxWidth, node.image.spriteLocation.width);
		}
		return Success();
	};

	auto computeWidthFitContentsText = [&]() -> ErrorOr<Success>
	{
		auto bounds = node.text.GetBoundsRequired(-1);
		dimensions.width = bounds.first;
		return Success();
	};

	auto computeHeightFitContentsImage = [&]() -> ErrorOr<Success>
	{
		int maxHeight = std::numeric_limits<int>::max();
		// rmf todo: could pass in explicitly whether parent width had been defined
		// this implies FitContents also fits parent, which I think is correct
		// since all (except mask, which is not yet implemented) children must fit their parents
		if (parent.height > 0)
		{
			int padding = dimensions.y;
			if (spec & NodeSpec::Bottom)
			{
				padding += CHECK_RETURN(ComputeScalar(
					window.height, parent.height, node.bottom));
			}
			maxHeight = std::min(maxHeight, parent.height - padding);
		}
		if (computedAttributes.count(DimensionAttribute::Width))
		{
			// rmf todo: 9sliced images (don't need to maintain ratio)
			float imageRatio =
				static_cast<float>(node.image.spriteLocation.height)
				/ static_cast<float>(node.image.spriteLocation.width);
			dimensions.height = static_cast<int>(round(dimensions.width * imageRatio));
			if (dimensions.height > maxHeight)
			{
				dimensions.height = maxHeight;
				// we should probably reduce width here, too?
				// but what if X has already been computed?
				// figure it out later
				return Error("Could not maintain aspect ratio of image " + node.image.filePath);
			}
		}
		else
		{
			dimensions.height = std::min(maxHeight, node.image.spriteLocation.height);
		}
		return Success();
	};

	auto computeHeightFitContentsText = [&]() -> ErrorOr<Success>
	{
		int maxWidth = -1;
		if (computedAttributes.count(DimensionAttribute::Width))
		{
			maxWidth = dimensions.width;
		}
		else
		{
			maxWidth = parent.width;
		}
		auto bounds = node.text.GetBoundsRequired(maxWidth);
		dimensions.height = bounds.second;
		return Success();
	};

	// because the dependencyOrdering has already been computed
	// we should be able to assume that the pre-requisites are available for use
	for (auto attribute : node.dependencyOrdering)
	{
		switch(attribute)
		{
		case DimensionAttribute::X:
		{
			if (spec & NodeSpec::Left)
			{
				dimensions.x = CHECK_RETURN(ComputeScalar(
					window.width, parent.width, node.left));
			}
			else if (spec & NodeSpec::Right)
			{
				// assume width is already computed if it's necessary
				int xRight = parent.width - CHECK_RETURN(ComputeScalar(
					window.width, parent.width, node.right));
				dimensions.x = xRight - dimensions.width;
			}
			// default is 0;
			break;
		}
		case DimensionAttribute::Y:
		{
			if (spec & NodeSpec::Top)
			{
				dimensions.x = CHECK_RETURN(ComputeScalar(
					window.height, parent.height, node.top));
			}
			else if (spec & NodeSpec::Bottom)
			{
				// assume height is already computed if it's necessary
				int yBottom = parent.height - CHECK_RETURN(ComputeScalar(
					window.height, parent.height, node.bottom));
				dimensions.y = yBottom - dimensions.height;
			}
			// default is 0;
			break;
		}
		case DimensionAttribute::Width:
		{
			if (spec & NodeSpec::Width)
			{
				switch(node.width.type)
				{
				case SizeType::Scalar:
					dimensions.width = CHECK_RETURN(ComputeScalar(
						window.width, parent.width, node.width.scalar));
					break;
				case SizeType::FitContents:
					if (!node.image.filePath.empty())
					{
						CHECK_RETURN(computeWidthFitContentsImage());
					}
					else if (!node.text.unparsedText.empty())
					{
						CHECK_RETURN(computeWidthFitContentsText());
					}
					break;
				case SizeType::FitChildren:
					int maxWidth = 0;
					int rightPad = std::numeric_limits<int>::max();
					for(const auto & childDim : dimensionsTree.children)
					{
						// rmf todo: bottom and right padding
						// for now assume they are the same as top and left
						rightPad = std::min(rightPad, childDim.value.x);
						maxWidth = std::max(maxWidth, childDim.value.x + childDim.value.width);
					}
					if (maxWidth == 0 || rightPad == std::numeric_limits<int>::max())
					{
						dimensions.width = 0;
					}
					else
					{
						dimensions.width = maxWidth + rightPad;
					}
					break;
				}
			}
			else if (spec & NodeSpec::Right)
			{
				int xRight = parent.width - CHECK_RETURN(ComputeScalar(
					window.width, parent.width, node.right));
				dimensions.width = xRight - dimensions.x;
			}
			else
			{
				// default is to fill parent
				dimensions.width = parent.width - dimensions.x;
			}
			break;
		}
		case DimensionAttribute::Height:
			if (spec & NodeSpec::Height)
			{
				switch(node.height.type)
				{
				case SizeType::Scalar:
					dimensions.height = CHECK_RETURN(ComputeScalar(
						window.height, parent.height, node.height.scalar));
					break;
				case SizeType::FitContents:
					if (!node.image.filePath.empty())
					{
						CHECK_RETURN(computeHeightFitContentsImage());
					}
					else if (!node.text.unparsedText.empty())
					{
						CHECK_RETURN(computeHeightFitContentsText());
					}
					break;
				case SizeType::FitChildren:
					int maxHeight = 0;
					int bottomPad = std::numeric_limits<int>::max();
					for(const auto & childDim : dimensionsTree.children)
					{
						// rmf todo: bottom and right padding
						// for now assume they are the same as top and left
						bottomPad = std::min(bottomPad, childDim.value.y);
						maxHeight = std::max(maxHeight, childDim.value.y + childDim.value.height);
					}
					if (maxHeight == 0 || bottomPad == std::numeric_limits<int>::max())
					{
						dimensions.height = 0;
					}
					else
					{
						dimensions.height = maxHeight + bottomPad;
					}
					break;
				}
			}
			else if (spec & NodeSpec::Bottom)
			{
				int yBottom = parent.height - CHECK_RETURN(ComputeScalar(
					window.height, parent.height, node.bottom));
				dimensions.height = yBottom - dimensions.y;
			}
			else
			{
				// default is to fill parent
				dimensions.height = parent.height - dimensions.y;
			}
			break;
		case DimensionAttribute::Children:
		{
			for (auto & child : node.children)
			{
				auto childTree = CHECK_RETURN(ComputeDimensions(
					window, dimensions, child));
				dimensionsTree.children.push_back(childTree);
			}
			break;
		}
		} // end switch(attribute)
		computedAttributes.insert(attribute);
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
	int parentAbsoluteX,
	int parentAbsoluteY,
	const Tree<Dimensions>& dimensions,
	const Node& node)
{
	Dimensions destination = dimensions.value;
	destination.x += parentAbsoluteX;
	destination.y += parentAbsoluteY;

	if (node.backgroundColor.a > 0)
	{
		tigrFill(
			window.get(),
			destination.x,
			destination.y,
			destination.width,
			destination.height,
			node.backgroundColor);
	}
	if (node.image.Defined())
	{
		CHECK_RETURN(node.image.Draw(window.get(), destination));
	}
	if (node.text.Defined())
	{
		CHECK_RETURN(node.text.Draw(window.get(), destination));
	}
	if (dimensions.children.size() != node.children.size())
	{
		return Error("Dimensions have different shape then ui node tree");
	}
	for (int i = 0; i < dimensions.children.size(); ++i)
	{
		CHECK_RETURN(Render(
			destination.x,
			destination.y,
			dimensions.children[i],
			node.children[i]));
	}
	return Success();
}

} // namespace UI


} // namespace Farb
