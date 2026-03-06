#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <ai/ai_message.h>
#include <persistance/imessage_repository.h>

class json_message_repository : public imessage_repository {
public:
	static constexpr int VERSION = 1;

public:
	json_message_repository(const std::string& file_path)
		: file_path_(file_path) {
		load_or_create();
	}
	~json_message_repository() = default;

	void reset_all() override {
		messages_.clear();
		save();
	}
	void reset_channel(int channel) override {
		messages_.erase(channel);
		save();
	}

	std::vector<ai_message> get_messages(int channel = 0) override;
	void save_message(const std::string& role, const std::string& content, int channel = 0) override;

private:
	std::string file_path_;
	std::unordered_map<int, std::vector<ai_message>> messages_;

	void load_or_create();
	void save();
};
