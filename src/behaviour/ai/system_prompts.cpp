#include <behaviour/ai/system_prompts.h>

#include <stdexcept>
#include <string>

std::string system_prompts::get_prompt(const std::string& mode) {
	std::string bhv;
	if (mode == "romantic") {
		bhv = romantic;
	}
	else if (mode == "confident") {
		bhv = confident;
	}
	else if (mode == "obsessive") {
		bhv = obsessive;
	}
	else if (mode == "shy") {
		bhv = shy_playful;
	}
	else if (mode == "postgame" || mode == "default" || mode.empty()) {
		bhv = postgame;
	}
	else {
		throw std::runtime_error("Unknown prompt preset: " + mode);
	}

	return bhv + "\n" + rules;
}
