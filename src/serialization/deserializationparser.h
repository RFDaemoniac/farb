#ifndef FARB_DESERIALIZATION_PARSER_H
#define FARB_DESERIALIZATION_PARSER_H

#include "../reflection/reflection.hpp"

namespace farb
{


class DeserializationParser
{
	// called when null is parsed
	bool null();

	// called when a boolean is parsed; value is passed
	bool boolean(bool val);

	// called when a signed or unsigned integer number is parsed; value is passed
	bool number_integer(number_integer_t val);
	bool number_unsigned(number_unsigned_t val);

	// called when a floating-point number is parsed; value and original string is passed
	bool number_float(number_float_t val, const string_t& s);

	// called when a string is parsed; value is passed and can be safely moved away
	bool string(string_t& val);

	// called when an object or array begins or ends, resp. The number of elements is passed (or -1 if not known)
	bool start_object(std::size_t elements);
	bool end_object();
	bool start_array(std::size_t elements);
	bool end_array();
	// called when an object key is parsed; value is passed and can be safely moved away
	bool key(string_t& val);

	// called when a parse error occurs; byte position, the last token, and an exception is passed
	bool parse_error(
		std::size_t position,
		const std::string& last_token,
		const detail::exception& ex);

} // class DeserializationParser

} // namespace farb

#endif