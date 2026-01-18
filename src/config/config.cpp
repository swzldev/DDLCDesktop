#include <config/config.h>

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

void config::load() {
	if (loaded_) {
		return; // already loaded
	}

	std::string path = "config.json";
	if (!fs::exists(path) || !fs::is_regular_file(path)) {
		load_default();
		return;
	}

	std::ifstream file(path);
	if (!file.is_open()) {
		load_default();
		return;
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
		load_default();
		return;
	}
	// API key
	cfg->api_key = j.value("api_key", "");
	// model
	cfg->model = j.value("model", "");
	// message history size
	cfg->message_history_size = j.value("message_history_size", 6);
	
	// pronouns
	cfg->pronouns = j.value("pronouns", "he/him");
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
		load_default();
		return;
	}

	// enable (ai) window controls
	cfg->enable_window_controls = j.value("enable_window_controls", true);

	loaded_ = std::move(cfg);
}
bool config::save() {
	if (!loaded_) {
		return false; // nothing to save
	}

	json j;

	// API
	switch (loaded_->api) {
	case api::OPENAI:
		j["api"] = "openai";
		break;
	case api::OPENROUTER:
		j["api"] = "openrouter";
		break;
	}
	// API key
	j["api_key"] = loaded_->api_key;
	// model
	j["model"] = loaded_->model;
	// message history size
	j["message_history_size"] = loaded_->message_history_size;
	// pronouns
	j["pronouns"] = loaded_->pronouns;
	// user name
	j["user_name"] = loaded_->user_name;
	// preset
	j["behaviour_preset"] = loaded_->behaviour_preset;
	// character
	switch (loaded_->character) {
	case ddlc_character::MONIKA:
		j["character"] = "monika";
		break;
	case ddlc_character::YURI:
		j["character"] = "yuri";
		break;
	case ddlc_character::NATSUKI:
		j["character"] = "natsuki";
		break;
	case ddlc_character::SAYORI:
		j["character"] = "sayori";
		break;
	}
	j["enable_window_controls"] = loaded_->enable_window_controls;

	std::string path = "config.json";
	std::ofstream file(path);
	if (!file.is_open()) {
		return false;
	}

	file << j.dump(4);
	file.close();

	return true;
}
config* config::get() {
	return loaded_.get();
}

void config::load_default() {
	loaded_ = std::make_unique<config>();
	
	// API
	loaded_->api = api::OPENROUTER;
	// API key
	loaded_->api_key = "";
	// model
	loaded_->model = "meta-llama/llama-3.3-70b-instruct:free";
	// message history size
	loaded_->message_history_size = 6;

	// pronouns
	loaded_->pronouns = "he/him";
	// user name
	loaded_->user_name = "User";
	// preset
	loaded_->behaviour_preset = "default";
	// character
	loaded_->character = ddlc_character::MONIKA;

	// enable (ai) window controls
	loaded_->enable_window_controls = true;
}

std::unique_ptr<config> config::loaded_ = nullptr;
