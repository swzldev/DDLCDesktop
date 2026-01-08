#include <config/config.h>

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

void config::load() {
	std::string path = "config.json";
	if (!fs::exists(path) || !fs::is_regular_file(path)) {
		throw std::runtime_error("Config file not found: " + path);
	}

	std::ifstream cfg(path);
	if (!cfg.is_open()) {
		throw std::runtime_error("Failed to open config file: " + path);
	}

	json j;
	cfg >> j;

	openai_api_key = j.value("openai_api_key", "");
	openai_model = j.value("openai_model", "gpt-4o-mini");
	openai_message_history_size = j.value("openai_message_history_size", 6);
	
	user_name = j.value("user_name", "");
	behaviour_preset = j.value("behaviour_preset", "");

	enable_resize = j.value("enable_resize", true);
	enable_move = j.value("enable_move", true);

	default_position = j.value("default_position", 1200);
	default_size = j.value("default_size", 450);

	cfg.close();
}
