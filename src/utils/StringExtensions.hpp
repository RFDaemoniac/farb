#ifndef FARB_STRING_EXTENSIONS_HPP
#define FARB_STRING_EXTENSIONS_HPP

#include <string>
#include <sstream>

namespace Farb
{

static bool EndsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size()
    	&& 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

static bool StartsWith(const std::string& str, const std::string& prefix)
{
    return str.size() >= prefix.size()
    	&& 0 == str.compare(0, prefix.size(), prefix);
}

static std::vector<std::string> Split(const std::string& str, char delimeter)
{
	std::stringstream ss(str);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delimeter)) {
		elems.push_back(std::move(item));
	}
	return elems;
}

} // namespace Farb

#endif // FARB_STRING_EXTENSIONS_HPP