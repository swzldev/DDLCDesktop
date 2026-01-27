#include <behaviour/memory/character_memory.h>

#include <fstream>

#include <nlohmann/json.hpp>

void character_memory::load() {
	std::ifstream file(MEMORY_FILE);
	if (!file.is_open()) {
		// no existing memory file; start fresh
		return;
	}

	nlohmann::json j;
	file >> j;
	file.close();

	message_history_ = message_history();
	for (const auto& item : j["history"]) {
		std::string role = item.value("role", "");
		std::string content = item.value("content", "");
		message_history_.add_message(role, content);
	}

	last_x = j.value("last_x", -1);
	last_y = j.value("last_y", -1);
	last_scale = j.value("last_scale", -1);
}
void character_memory::save() {
	nlohmann::json j;
	j["history"] = nlohmann::json::array();
	for (const auto& msg : message_history_.get_history()) {
		j["history"].push_back({ {"role", msg.role}, {"content", msg.content} });
	}
	j["last_x"] = last_x;
	j["last_y"] = last_y;
	j["last_scale"] = last_scale;

	std::ofstream file(MEMORY_FILE);
	if (file.is_open()) {
		file << j.dump(4);
		file.close();
	}
}
void character_memory::reset() {
	message_history_ = message_history();
	last_x = -1;
	last_y = -1;
	last_scale = -1;
}
