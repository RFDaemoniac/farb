#ifndef FARB_TYPE_INSPECTION_HPP
#define FARB_TYPE_INSPECTION_HPP

namespace Farb
{

// rmf todo: these were built under a specific use case for reflection
// might want to do some work to generalize them

template<typename T>
struct ExtractFunctionTypes
{ };

template<typename C, typename R, typename A>
struct ExtractFunctionTypes<R(C::*)(A)>
{
	using IsMember = std::true_type;
	using Class = C;
	using Return = R;
	using Param = A;
};

template<typename C, typename R, typename A>
struct ExtractFunctionTypes<R(*)(C&, A)>
{
	using IsMember = std::false_type;
	using Class = C;
	using Return = R;
	using Param = A;
};

template<typename C, typename R>
struct ExtractFunctionTypes<R(C::*)()>
{
	using IsMember = std::true_type;
	using Class = C;
	using Return = R;
};

template<typename C, typename R>
struct ExtractFunctionTypes<R(*)(C&)>
{
	using IsMember = std::false_type;
	using Class = C;
	using Return = R;
};


} // namespace Farb

#endif // FARB_TYPE_INSPECTION_HPP
