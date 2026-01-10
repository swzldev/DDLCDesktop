#include <output/log.h>

#include <stdexcept>
#include <fstream>
#include <chrono>
#include <format>

void log::open_file(const char* filename) {
	file_.open(filename, std::ios::out | std::ios::app);

	if (!file_.is_open()) {
		throw std::runtime_error("Failed to open log file");
	}

	// write start time
	auto now = std::chrono::system_clock::now();
	file_ << std::format("=== Log started at {:%Y-%m-%d %H:%M:%S} ===\n", std::chrono::floor<std::chrono::seconds>(now));
}
void log::shutdown() {
	if (file_.is_open()) {
		file_.close();
	}
}

std::ofstream log::file_;
