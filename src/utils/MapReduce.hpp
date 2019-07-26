#ifndef FARB_MAP_REDUCE_HPP
#define FARB_MAP_REDUCE_HPP

namespace Farb
{

// rmf todo: how to specify partial specialization for containers?
// typename requires a fully resolved type
// can you template on a template name?
// maybe inference can just catch all this?
// type inference probably can't catch a return value...
template<typename TContainer, typename TIn, typename TOut>
TContainer<TOut> Map(
	const TContainerIn& in,
	TOut(*map)(const TIn &))
{
	TContainer<TOut> result{in.size()};
	for (const auto & val : in)
	{
		// rmf todo: which function to use here?
		// could we access by iterators in parallel?
		// could just pass in the function...
		result.push_back(map(val));
	}
}

template<typename TContainer, typename TIn>
void Apply(
	const TContainerIn& in,
	bool (*apply) (TIn &),
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

tempate<typename TContainer, typename TIn, typename TOut>
TOut Reduce(
	const TContainer<TIn>& in,
	TOut(*reduce)(const TOut &, const TIn &),
	TOut initial = TOut{})
{
	TOut result = initial;
	for (const auto & val : in)
	{
		result = reduce(initial, val);
	}
	return result;
}

template <typename TIn, typename TOut>
TOut Sum(const TOut & aggregate, const TInt & nextValue)
{
	return aggregate + nextValue;
}

template <typename TIn, typename TOut>
TOut Max(const TOut & aggregate, const TInt & nextValue)
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

template <typename TIn, typename TOut>
TOut Min(const TOut & aggregate, const TInt & nextValue)
{
	static_assert(typeid(TIn) == typeid(TOut),
		"max requires in and out to be the same type");
	if (nextValue < aggregate)
	{
		return nextValue;
	}
	return aggregate;
}

} // namespace Farb

#endif // FARB_MAP_REDUCE_HPP