#ifndef FARB_INPUT_HANDLER_HPP
#define FARB_INPUT_HANDLER_HPP

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

struct Event
{
	InputType type;
	char key;
	std::pair<uint, uint> mousePosition;
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

	InputType inputType;
	Result(*)()

};

} // namespace Input

} // namespace Farb

#endif // FARB_INPUT_HANDLER_HPP