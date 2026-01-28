#include <iostream>

int main(int argc, char** argv) {
	if (argc > 2 || argc < 1) {
		std::cerr << "Usage: Updater [optional-version]" << std::endl;
		return 1;
	}

	std::string version = (argc == 2) ? argv[1] : "";
}