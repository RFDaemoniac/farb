#ifndef FARB_INT_EXTENSIONS_HPP
#define FARB_INT_EXTENSIONS_HPP

namespace Farb
{

constexpr int Pow(int a, uint b)
{
	int result = 1;
	while (b > 0)
	{
		result *= a;
		b--;
	}
	return result;
}

constexpr int Round(float f)
{
	int value = 0;
	if (f < 0.0)
	{
		value = static_cast<int>(f - 0.5f);
	}
	else
	{
		value = static_cast<int>(f + 0.5f);
	}
	return value;
}

} // namespace Farb

#endif // FARB_INT_EXTENSIONS_HPP