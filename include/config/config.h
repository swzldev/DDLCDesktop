#pragma once

#include <string>

class config {
public:
	static void load();

	static std::string openai_api_key;
	static std::string openai_model;
	static int openai_message_history_size;

	static std::string user_name;
	static std::string behaviour_preset;

	static bool enable_resize;
	static bool enable_move;

	static int default_position;
	static int default_size;
};