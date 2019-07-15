#ifndef FARB_WINDOW_H
#define FARB_WINDOW_H

#include <string>

#include "UINode.h"
#include "../core/ErrorOr.hpp"
#include "../core/Containers.hpp"

namespace Farb
{

namespace UI
{

struct Window
{
	std::unique_ptr<Tigr, TigrDeleter> window;

	Window(int width, int height, std::string name);

	bool Render(const Node& tree);

private:
	ErrorOr<Success> Render(
		const Tree<Dimensions>& dimensions,
		const Node& node);

	ErrorOr<Tree<Dimensions> > ComputeDimensions(
		const Dimensions& window,
		const Dimensions& parent,
		const Node& node);
};

} // namespace UI

} // namespace Farb

#endif // FARB_WINDOW_H