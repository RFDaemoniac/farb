#ifndef FARB_INPUT_HANDLER_HPP
#define FARB_INPUT_HANDLER_HPP

#include "../core/NamedType.hpp"

namespace Farb
{

namespace Input
{

enum class Type
{
	MouseDown,
	MouseUp,
	MouseClick,
	KeyDown,
	KeyUp,
	KeyPress
};

struct PixelTag
{
	static HString GetName() { return "Pixel"; }
};

using Pixel = NamedType<uint, PixelTag>;

struct WindowCoordinate
{
	Pixel x, y;
};

struct Event
{
	Type type;
	char key;
	WindowCoordinate mousePosition;
};

struct Handler
{
	struct Result
	{
		bool consume;
		bool responded;

		Result(bool both)
			: consume(both)
			, responded(both)
		{ }
	};

	Type inputType;
	Result (*response)(Event);
};

} // namespace Input

} // namespace Farb

#endif // FARB_INPUT_HANDLER_HPP