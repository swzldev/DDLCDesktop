#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <format>

class log {
public:
	static void open_file(const char* filename);
	static void shutdown();

	template<typename... Args>
	static void print(std::string_view message, Args&&... args);

private:
	static std::ofstream file_;
};

template<typename... Args>
inline void log::print(std::string_view message, Args&&... args) {
	if (file_.is_open()) {
		file_ << std::vformat(message, std::make_format_args(args...)) << std::endl;
	}
}
