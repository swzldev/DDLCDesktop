#include <persistance/json_message_repository.h>

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

#include <error/internal.h>

std::vector<ai_message> json_message_repository::get_messages(int channel) {
	if (messages_.find(channel) != messages_.end()) {
		return messages_[channel];
	}
	return {};
}
void json_message_repository::save_message(int channel, const std::string& role, const std::string& content) {
	messages_[channel].push_back({ role, content });
	save();
}

void json_message_repository::load_or_create()
{
	if (!std::filesystem::exists(file_path_)) {
		save();
	}
	else {
		std::ifstream file(file_path_);
		nlohmann::json j;
		file >> j;

		int version = j.value("version", -1);
		if (version == -1 || version >= VERSION) {
			d_panic("Unsupported message repository version! Please delete " + file_path_ + " to reset!");
		}

		nlohmann::json channels_json = j.value("channels", nlohmann::json::array());
		for (const auto& channel_json : channels_json) {
			int channel_id = channel_json.value("id", -1);
			if (channel_id == -1) {
				continue;
			}

			std::vector<ai_message> messages;
			nlohmann::json messages_json = channel_json.value("messages", nlohmann::json::array());
			for (const auto& message_json : messages_json) {
				std::string role = message_json.value("role", "");
				std::string content = message_json.value("content", "");
				if (!role.empty() && !content.empty()) {
					messages.push_back({ role, content });
				}
			}
			messages_[channel_id] = messages;
		}
	}
}
void json_message_repository::save()
{
	nlohmann::json j;
	j["version"] = VERSION;
	
	std::vector<nlohmann::json> channels_json;
	for (const auto& [channel_id, messages] : messages_) {
		nlohmann::json channel_json;
		// channel id
		channel_json["id"] = channel_id;

		// channel messages
		std::vector<nlohmann::json> messages_json;
		for (const auto& message : messages) {
			nlohmann::json message_json;
			message_json["role"] = message.role;
			message_json["content"] = message.content;
			messages_json.push_back(message_json);
		}
		channel_json["messages"] = messages_json;
		
		channels_json.push_back(channel_json);
	}
}
