#include <output/log.h>

#include <stdexcept>
#include <fstream>

void log::open_file(const char* filename) {
	file_.open(filename, std::ios::out);

	if (!file_.is_open()) {
		throw std::runtime_error("Failed to open log file");
	}
}
void log::shutdown() {
	if (file_.is_open()) {
		file_.close();
	}
}

std::ofstream log::file_;
