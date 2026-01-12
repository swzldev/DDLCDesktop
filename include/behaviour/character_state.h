#pragma once

#include <string>
#include <vector>

struct character_state {
	struct interaction {
		std::string saying;
		std::string expression;
		std::string pose_left;
		std::string pose_right;
		int new_x = -1;
		int new_scale = -1;
	};

	enum class error {
		NONE,
		FAIL_PARSE_RESPONSE_JSON,
		FAIL_PARSE_RESPONSE_UNKNOWN,
	};

	error err = error::NONE;
	std::vector<interaction> interactions;
	std::string style;
	std::vector<std::string> actions;
};