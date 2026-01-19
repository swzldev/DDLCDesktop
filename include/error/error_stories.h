#pragma once

#include <vector>
#include <string>

#include <behaviour/character_state.h>

namespace error_stories {
	std::vector<character_state::interaction> firstrun_story_p1();
	std::vector<character_state::interaction> firstrun_story_p2(const std::string& name);
	std::vector<character_state::interaction> firstrun_story_p3(const std::string& name);
	std::vector<character_state::interaction> firstrun_story_p4(const std::string& name);
	std::vector<character_state::interaction> fail_load_config_story();
	std::vector<character_state::interaction> fail_ai_response_story();
	std::vector<character_state::interaction> fail_parse_ai_response_story();
}