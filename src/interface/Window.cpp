#include <cmath>
#include <limits>

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

	std::set<DimensionAttributes> computedAttributes;

	auto computeWidthFitContentsImage = [=]() -> ErrorOr<Success>
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
			maxWidth = min(maxWidth, parent.width - padding);
		}
		if (computedAttributes.count(DimensionAttributes::Height))
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
				return Error("Could not maintain aspect ratio of image " + node.image.filePath());
			}
		}
		else
		{
			dimensions.width = min(maxWidth, node.image.spriteLocation.width);
		}
		return Success();
	}

	auto computeWidthFitContentsText = [=]() -> ErrorOr<Success>
	{
		return Error("Text width FitContents not implemented");
	}

	auto computeHeightFitContentsImage = [=]() -> ErrorOr<Success>
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
			maxHeight = min(maxHeight, parent.height - padding);
		}
		if (computedAttributes.count(DimensionAttributes::Width))
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
				return Error("Could not maintain aspect ratio of image " + node.image.filePath());
			}
		}
		else
		{
			dimensions.height = min(maxHeight, node.image.spriteLocation.height);
		}
		return Success();
	}

	auto computeHeightFitContentsText = [=]() -> ErrorOr<Success>
	{
		return Error("Text height FitContents not implemented");
	}

	// because the dependencyOrdering has already been computed
	// we should be able to assume that the pre-requisites are available for use
	for (auto attribute : node.dependencyOrdering)
	{
		switch(attribute)
		{
		case DimensionAttributes::X:
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
		case DimensionAttributes::Y:
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
		case DimensionAttributes::Width:
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
						rightPad = min(rightPad, childDim.x);
						maxWidth = max(maxWidth, childDim.x + childDim.width);
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
		case DimensionAttributes::Height:
			if (spec & NodeSpec::Height)
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
						bottomPad = min(bottomPad, childDim.y);
						maxHeight = max(maxHeight, childDim.y + childDim.height);
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
		case DimensionAttributes::Children:
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