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
			bhv = monika_shy;
		}
		else if (mode == "postgame" || mode == "default" || mode.empty()) {
			bhv = monika_postgame;
		}
		else {
			throw std::runtime_error("(Monika) Unknown prompt preset: " + mode);
		}
		bhv += std::string("\n") + "Monika expressions: " + monika_expressions;
		bhv += std::string("\n") + "Monika poses: " + monika_poses;
		break;
	}
	case ddlc_character::YURI: {
		if (mode == "postgame" || mode == "default" || mode.empty()) {
			bhv = yuri_postgame;
		}
		else if (mode == "romantic") {
			bhv = yuri_romantic;
		}
		else if (mode == "obsessive") {
			bhv = yuri_obsessive;
		}
		else if (mode == "shy") {
			bhv = yuri_shy;
		}
		else {
			throw std::runtime_error("(Yuri) Unknown prompt preset: " + mode);
		}
		bhv += std::string("\n") + "Yuri expressions: " + yuri_expressions;
		bhv += std::string("\n") + "Yuri poses: " + yuri_poses;
		break;
	}
	case ddlc_character::NATSUKI: {
		if (mode == "postgame" || mode == "default" || mode.empty()) {
			bhv = natsuki_postgame;
		}
		else if (mode == "romantic") {
			bhv = natsuki_romantic;
		}
		else if (mode == "hurt") {
			bhv = natsuki_hurt;
		}
		else if (mode == "playful") {
			bhv = natsuki_playful;
		}
		else {
			throw std::runtime_error("(Natsuki) Unknown prompt preset: " + mode);
		}
		bhv += std::string("\n") + "Natsuki expressions: " + natsuki_expressions;
		bhv += std::string("\n") + "Natsuki poses: " + natsuki_poses;
		break;
	}
	case ddlc_character::SAYORI: {
		if (mode == "postgame" || mode == "default" || mode.empty()) {
			bhv = sayori_postgame;
		}
		else if (mode == "depressed") {
			bhv = sayori_depressed;
		}
		else if (mode == "romantic") {
			bhv = sayori_romantic;
		}
		else if (mode == "happy") {
			bhv = sayori_happy;
		}
		else if (mode == "obsessive") {
			bhv = sayori_obsessive;
		}
		else {
			throw std::runtime_error("(Sayori) Unknown prompt preset: " + mode);
		}
		bhv += std::string("\n") + "Sayori expressions: " + sayori_expressions;
		bhv += std::string("\n") + "Sayori poses: " + sayori_poses;
		break;
	}
	default:
		throw std::runtime_error("Unknown character for system prompt");
	}

	return bhv + "\n" + rules;
}
