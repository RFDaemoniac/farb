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

} // namespace Farb

#endif // FARB_INT_EXTENSIONS_HPP