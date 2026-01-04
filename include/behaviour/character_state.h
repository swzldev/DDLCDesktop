#pragma once

#include <string>
#include <vector>

struct character_state {
	struct interaction {
		std::string saying;
		std::string visual;
	};

	std::vector<interaction> interactions;
	std::vector<std::string> actions;
};