#ifndef FARB_CONTAINERS_HPP
#define FARB_CONTAINERS_HPP

#include <vector>
#include "Error.hpp"

namespace Farb
{

template<typename T>
struct Tree
{
	T value;
	std::vector<Tree> children;

	Tree& GetAtPath(std::vector<int>& path, int pathIndex = 0)
	{
		if (path.size() <= pathIndex)
		{
			Error("Tree GetAtPath pathIndex is out of range of treeIndexes").Log();
			return *this;
		}
		if (children.size() <= path[pathIndex])
		{
			Error("Tree GetAtPath treeIndex is out of range of children").Log();
			return *this;
		}
		if (path.size() - 1 == pathIndex)
		{
			return children[path[pathIndex]];
		}
		else
		{
			return children[path[pathIndex]].GetAtPath(path, pathIndex + 1);
		}
	}
};

} // namespace Farb

#endif // FARB_CONTAINERS_HPP