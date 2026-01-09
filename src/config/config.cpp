#include <config/config.h>

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

config config::load() {
	config cfg;
	std::string path = "config.json";
	if (!fs::exists(path) || !fs::is_regular_file(path)) {
		throw std::runtime_error("Config file not found: " + path);
	}

	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open config file: " + path);
	}

	json j;
	file >> j;

	cfg.openai_api_key = j.value("openai_api_key", "");
	cfg.openai_model = j.value("openai_model", "gpt-4o-mini");
	cfg.openai_message_history_size = j.value("openai_message_history_size", 6);
	
	cfg.user_name = j.value("user_name", "");
	cfg.behaviour_preset = j.value("behaviour_preset", "");

	cfg.enable_resize = j.value("enable_resize", true);
	cfg.enable_move = j.value("enable_move", true);

	cfg.default_position = j.value("default_position", 1200);
	cfg.default_size = j.value("default_size", 450);

	file.close();

	return cfg;
}
