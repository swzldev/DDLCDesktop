#pragma once

#include <string>

class config {
public:
	static config load();

	std::string openai_api_key;
	std::string openai_model;
	int openai_message_history_size;

	std::string user_name;
	std::string behaviour_preset;

	bool enable_resize;
	bool enable_move;

	int default_position;
	int default_size;
};