#ifndef FARB_TEST_ERROR_OR_HPP
#define FARB_TEST_ERROR_OR_HPP

#include "ErrorOr.hpp"

namespace Farb
{

struct Obj
{
	std::shared_ptr<int> a;
};

ErrorOr<Obj> TryGet()
{
	ErrorOr<Obj> a = Error("broken");

	ErrorOr<int> b = a.GetError();
	return b.GetError();
}

namespace Tests
{

class TestErrorOr : public ITest
{
public:
	virtual bool RunTests() const override
	{
		std::cout << "ErrorOr" << std::endl;


		ErrorOr<Obj> a = Obj{};
		a = Error("broken");
		Error b = a.GetError();

		ErrorOr<int> b2 = a.GetError();

		b.Log();
		b2.GetError().Log();
		


		return true;
	}
};

} // namespace Tests


} // namespace Farb

#endif // FARB_TEST_ERROR_OR_HPP