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
	try {
		file >> j;
	}
	catch (const json::parse_error&) {
		file.close();
		load_default();
		return;
	}
	file.close();

	std::unique_ptr<config> cfg = std::make_unique<config>();

	// application
	cfg->start_on_boot = j.value("start_on_boot", false);
	cfg->run_in_background = j.value("run_in_background", false);
	cfg->auto_mode_speed = std::clamp(j.value("auto_mode_speed", 3), 1, 5);
	cfg->enable_discord_rpc = j.value("enable_discord_rpc", true);

	// ai
	std::string api_str = j.value("api", "");
	if (api_str == "openai") {
		cfg->api = api::OPENAI;
	}
	else if (api_str == "openrouter") {
		cfg->api = api::OPENROUTER;
	}
	else if (api_str == "custom") {
		cfg->api = api::CUSTOM;
	}
	else {
		load_default();
		return;
	}
	// API key
	cfg->api_key = j.value("api_key", "");
	// model
	cfg->model = j.value("model", "");
	// custom endpoint
	cfg->custom_endpoint = j.value("custom_endpoint", "");
	// message history size
	cfg->message_history_size = j.value("message_history_size", 6);
	// max tokens
	cfg->max_tokens = j.value("max_tokens", 2000);

	// pronouns
	cfg->pronouns = j.value("pronouns", "he/him");
	// user name
	cfg->user_name = j.value("user_name", "");
	// language
	cfg->language = j.value("language", "English");
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
	case api::CUSTOM:
		j["api"] = "custom";
		break;
	}
	// application
	j["run_on_boot"] = loaded_->start_on_boot;
	j["run_in_background"] = loaded_->run_in_background;
	j["auto_mode_speed"] = loaded_->auto_mode_speed;
	j["enable_discord_rpc"] = loaded_->enable_discord_rpc;
	// ai
	j["api_key"] = loaded_->api_key;
	j["model"] = loaded_->model;
	j["custom_endpoint"] = loaded_->custom_endpoint;
	j["message_history_size"] = loaded_->message_history_size;
	j["max_tokens"] = loaded_->max_tokens;
	// user
	j["pronouns"] = loaded_->pronouns;
	j["user_name"] = loaded_->user_name;
	j["language"] = loaded_->language;
	// behaviour
	j["behaviour_preset"] = loaded_->behaviour_preset;
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
	if (!loaded_) {
		load();
	}

	return loaded_.get();
}

void config::load_default() {
	loaded_ = std::make_unique<config>();

	// application
	loaded_->start_on_boot = false;
	loaded_->run_in_background = false;
	loaded_->auto_mode_speed = 3;
	loaded_->enable_discord_rpc = true;

	// API
	loaded_->api = api::OPENROUTER;
	// API key
	loaded_->api_key = "";
	// model
	loaded_->model = "meta-llama/llama-3.3-70b-instruct";
	// custom endpoint
	loaded_->custom_endpoint = "";
	// message history size
	loaded_->message_history_size = 6;

	// pronouns
	loaded_->pronouns = "he/him";
	// user name
	loaded_->user_name = "User";
	// language
	loaded_->language = "English";
	// preset
	loaded_->behaviour_preset = "default";
	// character
	loaded_->character = ddlc_character::MONIKA;

	// enable (ai) window controls
	loaded_->enable_window_controls = true;
}

std::unique_ptr<config> config::loaded_ = nullptr;
