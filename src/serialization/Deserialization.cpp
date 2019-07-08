#include <fstream>
#include <iostream>
#include <stack>
#include <string>

#include "../../lib/json/json.hpp"

#include "Deserialization.h"

namespace Farb
{

using json = nlohmann::json;
using namespace Reflection;

struct ReflectionContext
{
	ReflectionObject reflect;
	int arrayIndex;
	bool inObject;
	bool inArray;

	// could put function to call here after being done as an optional member
	// which can be used for constructing the object not in place and then copying

	// should also implement shared values here

	ReflectionContext(ReflectionObject reflect)
		: reflect(reflect)
		, arrayIndex(-1)
		, inObject(false)
		, inArray(false)
	{ }
};

class DeserializationParser : public json::json_sax_t
{
public:
	std::stack<ReflectionContext> stack;

	DeserializationParser(ReflectionObject root)
		: stack()
	{
		stack.push(ReflectionContext(root));
	}

	// called when null is parsed
	bool null()
	{
		// rmf todo: @implement null
		Error("json null not implemented").Log();
		return false;
	}

	// called when a boolean is parsed; value is passed
	bool boolean(bool val)
	{
		if (!UpkeepForValueStart()) { return false; }
		bool success = stack.top().reflect.AssignBool(val);
		stack.pop();
		return success;
	}

	// called when a signed or unsigned integer number is parsed; value is passed
	bool number_integer(number_integer_t val)
	{
		if (!UpkeepForValueStart()) { return false; }
		bool success = stack.top().reflect.AssignInt(val);
		stack.pop();
		return success;
	}

	bool number_unsigned(number_unsigned_t val)
	{
		if (!UpkeepForValueStart()) { return false; }
		bool success = stack.top().reflect.AssignUInt(val);
		stack.pop();
		return success;
	}

	// called when a floating-point number is parsed; value and original string is passed
	bool number_float(number_float_t val, const string_t& s)
	{
		if (!UpkeepForValueStart()) { return false; }
		bool success = stack.top().reflect.AssignFloat(val);
		stack.pop();
		return success;
	}

	// called when a string is parsed; value is passed and can be safely moved away
	bool string(string_t& val)
	{
		if (!UpkeepForValueStart()) { return false; }
		bool success = stack.top().reflect.AssignString(val);
		stack.pop();
		return success;
	}

	// called when an object or array begins or ends, resp. The number of elements is passed (or -1 if not known)
	bool start_object(std::size_t elements)
	{
		if (!UpkeepForValueStart()) { return false; }
		stack.top().inObject = true;
		return true;
	}

	bool end_object()
	{
		if (stack.empty()) { return false; }
		if (!stack.top().inObject) { return false; }
		stack.top().inObject = false;
		stack.pop();
		return true;
	}

	bool start_array(std::size_t elements)
	{
		if (!UpkeepForValueStart()) { return false; }
		stack.top().inArray = true;
		return true;
	}

	bool end_array()
	{
		if (stack.empty()) { return false; }
		stack.top().inArray = false;
		stack.pop();
		return true;
	}

	// called when an object key is parsed; value is passed and can be safely moved away
	bool key(string_t& val)
	{
		if (stack.empty()) { return false; }
		if (!stack.top().inObject) { return false; }
		bool success = stack.top().reflect.InsertKey(val);
		if (!success) { return false; }
		auto result = stack.top().reflect.GetAtKey(val);
		if (result.IsError())
		{
			result.GetError().Log();
			return false;
		}
		stack.push(ReflectionContext(result.GetValue()));
		return true;
	}

	// called when a parse error occurs; byte position, the last token, and an exception is passed
	bool parse_error(
		std::size_t position,
		const std::string& last_token,
		const nlohmann::detail::exception& ex)
	{
		Error(
			"1 Parse Error into object of type "
			+ stack.top().reflect.typeInfo->GetName()
			+ "\n last token was: "
			+ last_token
			+ "\n at: "
			+ std::to_string(position)
			+ "\n exception: "
			+ ex.what()).Log();
		return false;
	}

protected:
	bool UpkeepForValueStart()
	{
		if (stack.empty())
		{
			return false;
		}
		if (stack.top().inArray && !ArrayNextSetup())
		{
			return false;
		}
		return true;
	}
	// false implies error
	// true implies continue after having setup the child context
	bool ArrayNextSetup()
	{
		bool success = stack.top().reflect.PushBackDefault();
		if (!success)
		{
			 return false;
		}
		stack.top().arrayIndex++;

		auto result = stack.top().reflect.GetAtIndex(stack.top().arrayIndex);
		if (result.IsError())
		{
			result.GetError().Log();
			return false;
		}
		stack.push(ReflectionContext(result.GetValue()));
		return true;
	}

}; // class DeserializationParser

bool DeserializeString(std::string input, ReflectionObject reflect)
{
	DeserializationParser parser(reflect);
	return json::sax_parse(input, &parser);
}

bool DeserializeFile(std::string filePath, ReflectionObject reflect)
{
	DeserializationParser parser(reflect);
	std::ifstream inputFile(filePath);
	if (inputFile.fail())
	{
		Error("Couldn't open filePath: " + filePath).Log();
		return false;
	}
	return json::sax_parse(inputFile, &parser);
}

} // namespace Farb
