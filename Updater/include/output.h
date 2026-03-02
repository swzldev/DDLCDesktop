#pragma once

#include <string>
#include <sstream>
#include <format>
#include <iostream>

namespace output {
	template<typename... Args>
	void print(const std::string& format_str, Args... args);

	std::string readline();
}

template<typename... Args>
void output::print(const std::string& format_str, Args... args) {
	std::string formatted_str = std::vformat(format_str, std::make_format_args(args...));
	std::cout << formatted_str << std::endl;
}
