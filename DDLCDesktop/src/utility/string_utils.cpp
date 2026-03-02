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