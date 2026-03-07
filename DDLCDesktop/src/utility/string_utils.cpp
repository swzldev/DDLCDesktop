#include <utility/string_utils.h>

#include <string>
#include <algorithm>

std::string string_utils::to_lower(const std::string& str) {
	std::string lower_str = str;
	std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), [](unsigned char c) {
		return std::tolower(c);
	});
	return lower_str;
}
std::string string_utils::trim(const std::string& str) {
	const std::string whitespace = " \t\n\r\f\v";
	size_t start = str.find_first_not_of(whitespace);
	if (start == std::string::npos) {
		return ""; // string is all whitespace
	}
	size_t end = str.find_last_not_of(whitespace);
	return str.substr(start, end - start + 1);
}
