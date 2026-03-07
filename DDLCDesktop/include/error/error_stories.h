#pragma once

#include <vector>
#include <string>

#include <behaviour/character_state.h>

namespace error_stories {
	std::vector<character_state::interaction> firstrun_story_p1();
	std::vector<character_state::interaction> firstrun_story_p2(const std::string& name);
	std::vector<character_state::interaction> firstrun_story_p3();
	std::vector<character_state::interaction> firstrun_story_p4();

	std::vector<character_state::interaction> update_story(const std::string& name);
	std::vector<character_state::interaction> error_story(const std::string& user_name, const std::string& message);
}