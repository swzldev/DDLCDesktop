#include <output/log.h>

#include <stdexcept>
#include <fstream>
#include <chrono>
#include <format>
#include <filesystem>

namespace fs = std::filesystem;

void log::open_file(const char* filename) {
	if (file_.is_open()) {
		file_.close();
	}

	const std::uintmax_t max_size_bytes = 5 * 1024 * 1024; // 5mb
	try {
		if (fs::exists(filename)) {
			const auto size = fs::file_size(filename);
			if (size > max_size_bytes) {
				const std::string old_name = std::string(filename) + ".old";
				std::error_code ec;
				fs::rename(filename, old_name, ec);
			}
		}
	}
	catch (...) {
		// ignore
	}

	file_.open(filename, std::ios::out | std::ios::app);

	if (!file_.is_open()) {
		throw std::runtime_error("Failed to open log file");
	}

	// write start time
	auto now = std::chrono::system_clock::now();
	file_ << std::format(
		"=== Log started at {:%Y-%m-%d %H:%M:%S} ===\n",
		std::chrono::floor<std::chrono::seconds>(now));
}
void log::shutdown() {
	if (file_.is_open()) {
		file_.close();
	}
}

std::ofstream log::file_;
