#include <behaviour/ai/system_prompts.h>

#include <stdexcept>
#include <string>

#include <ddlc/characters.h>

std::string system_prompts::get_prompt(ddlc_character character, const std::string& mode) {
	std::string bhv;

	switch (character) {
	case ddlc_character::MONIKA: {
		if (mode == "romantic") {
			bhv = monika_romantic;
		}
		else if (mode == "confident") {
			bhv = monika_confident;
		}
		else if (mode == "obsessive") {
			bhv = monika_obsessive;
		}
		else if (mode == "shy") {
			bhv = monika_shy_playful;
		}
		else if (mode == "postgame" || mode == "default" || mode.empty()) {
			bhv = monika_postgame;
		}
		else {
			throw std::runtime_error("(Monika) Unknown prompt preset: " + mode);
		}
		
		return bhv + "\n" + rules_monika;
	}
	case ddlc_character::YURI: {
		if (mode == "postgame" || mode == "default" || mode.empty()) {
			bhv = yuri_postgame;
		}
		else {
			throw std::runtime_error("(Yuri) Unknown prompt preset: " + mode);
		}

		return bhv + "\n" + rules_yuri;
	}
	}
}
