#ifndef FARB_WINDOW_H
#define FARB_WINDOW_H

#include "UINode.h"
#include "../core/ErrorOr.hpp"
#include "../core/Containers.hpp"

namespace Farb
{

namespace UI
{

struct Window
{
	std::unique_ptr<Tigr, tigrFree> window;

	bool Render(const Node& tree);

private:
	ErrorOr<void> Render(
		const Tree<ComputedDimensions>& dimensions,
		const Node& node);

	ErrorOr<ComputedDimensions> ComputeDimensions(
		const ComputedDimensions& window,
		const ComputedDimensions& parent,
		const Node& node);
}

} // namespace UI

} // namespace Farb

#endif // FARB_WINDOW_H