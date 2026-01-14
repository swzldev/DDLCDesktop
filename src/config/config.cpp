#include <config/config.h>

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

bool config::load() {
	if (loaded_) {
		return true; // already loaded
	}

	std::string path = "config.json";
	if (!fs::exists(path) || !fs::is_regular_file(path)) {
		return false;
	}

	std::ifstream file(path);
	if (!file.is_open()) {
		return false;
	}

	json j;
	file >> j;
	file.close();

	std::unique_ptr<config> cfg = std::make_unique<config>();

	// API
	std::string api_str = j.value("api", "");
	if (api_str == "openai") {
		cfg->api = api::OPENAI;
	}
	else if (api_str == "openrouter") {
		cfg->api = api::OPENROUTER;
	}
	else {
		return false;
	}
	// API key
	cfg->api_key = j.value("api_key", "");
	// model
	cfg->model = j.value("model", "");
	// message history size
	cfg->message_history_size = j.value("message_history_size", 6);
	
	// user name
	cfg->user_name = j.value("user_name", "");
	// preset
	cfg->behaviour_preset = j.value("behaviour_preset", "");
	// character
	std::string character_str = j.value("character", "monika");
	if (character_str == "monika") {
		cfg->character = ddlc_character::MONIKA;
	}
	else if (character_str == "yuri") {
		cfg->character = ddlc_character::YURI;
	}
	else if (character_str == "natsuki") {
		cfg->character = ddlc_character::NATSUKI;
	}
	else if (character_str == "sayori") {
		cfg->character = ddlc_character::SAYORI;
	}
	else {
		return false;
	}

	loaded_ = std::move(cfg);
	return true;
}
const config* config::get() {
	return loaded_.get();
}

std::unique_ptr<config> config::loaded_ = nullptr;
