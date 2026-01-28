#include <output.h>

#include <string>
#include <iostream>
#include <sstream>

std::string output::readline() {
	std::string input;
	std::getline(std::cin, input);
	return input;
}
