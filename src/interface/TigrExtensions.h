#ifndef FARB_TIGR_EXTENSIONS_H
#define FARB_TIGR_EXTENSIONS_H

#include "../../lib/tigr/tigr.h"

namespace Farb
{

namespace UI
{

struct TigrDeleter
{
	void operator()(Tigr* bmp) { if (bmp != nullptr) tigrFree(bmp); }
};

struct Dimensions
{
	int x, y;
	int width, height;

	Dimensions()
		: x(0), y(0), width(0), height(0)
	{ }

	Dimensions(int x, int y, int width, int height)
		: x(x), y(y), width(width), height(height)
	{ }
};

enum class DimensionAttribute
{
	X,
	Y,
	Width,
	Height,
	Children
};

} // namespace UI


} // namespace Farb

#endif // FARB_TIGR_EXTENSIONS_H