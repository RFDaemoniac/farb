#ifndef FARB_MAP_REDUCE_HPP
#define FARB_MAP_REDUCE_HPP

namespace Farb
{

template<typename TRet, typename ...TArgs>
struct Functor
{
	virtual TRet operator()(TArgs... args);
};

template<typename ...TArgs>
struct Functor<void, TArgs...>
{
	virtual void operator()(TArgs... args);
};

template<typename TRet, typename ...TArgs>
struct FunctionPointer : public Functor<TRet, TArgs...>
{
	TRet (*func)(TArgs...);

	FunctionPointer(TRet (*func)(TArgs...))
		:func(func)
	{ }
};

// rmf todo: how to specify partial specialization for containers?
// typename requires a fully resolved type
// can you template on a template name?
// maybe inference can just catch all this?
// type inference probably can't catch a return value...
template<template<typename, typename...> typename TContainer, typename TIn, typename TOut, typename ... TInArgs>
TContainer<TOut, TInArgs...> Map(
	const TContainer<TIn, TInArgs...>& in,
	Functor<TOut, const TIn &> & map)
{
	TContainer<TOut, TInArgs...> result{in.size()};
	for (const auto & val : in)
	{
		// rmf todo: which function to use here?
		// could we access by iterators in parallel?
		// could just pass in the function...
		result.push_back(map(val));
	}
}

template<template<typename, typename...> typename TContainer, typename TIn, typename ...TInArgs>
bool Apply(
	const TContainer<TIn, TInArgs...>& in,
	Functor<bool, TIn &> & apply,
	bool breakOnFailure = false)
{
	bool success = true;
	for (auto & val : in)
	{
		success = success && apply(val);
		if (breakOnFailure && !success)
		{
			return false;
		}
	}
	return success;
}

template<template<typename, typename...> typename TContainer, typename TIn, typename TOut, typename ... TInArgs>
TOut Reduce(
	const TContainer<TIn, TInArgs...> & in,
	Functor<TOut, const TOut &, const TIn &> & reduce,
	TOut initial = TOut{})
{
	TOut result = initial;
	for (const auto & val : in)
	{
		result = reduce(result, val);
	}
	return result;
}

template <typename TIn, typename TOut>
struct Sum : Functor<TOut, const TOut &, const TIn &>
{
	virtual TOut operator()(const TOut & aggregate, const TIn & nextValue) override
	{
		return aggregate + nextValue;
	}
};

template <typename TIn, typename TOut>
struct Max : Functor<TOut, const TOut &, const TIn &>
{
	virtual TOut operator()(const TOut & aggregate, const TIn & nextValue) override
	{
		static_assert(typeid(TIn) == typeid(TOut),
			"max requires in and out to be the same type");
		if (nextValue > aggregate)
		{
			// rmf note: do I need to static cast here?
			return nextValue;
		}
		return aggregate;
	}
};

template <typename TIn, typename TOut>
struct Min : Functor<TOut, const TOut &, const TIn &>
{
	virtual TOut operator()(const TOut & aggregate, const TIn & nextValue) override
	{
		static_assert(typeid(TIn) == typeid(TOut),
			"max requires in and out to be the same type");
		if (nextValue < aggregate)
		{
			return nextValue;
		}
		return aggregate;
	}
};

} // namespace Farb

#endif // FARB_MAP_REDUCE_HPP