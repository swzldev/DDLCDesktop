#pragma once

#include <string>

#include <behaviour/memory/message_history.h>

class character_memory {
public:
	static constexpr const char* MEMORY_FILE = "character_memory.json";

public:
	character_memory() = default;

	void load();
	void save();
	void reset();

	message_history& get_history() {
		return message_history_;
	}

	int last_x;
	int last_y;
	int last_scale;
	std::string last_expression;
	std::string last_pose;

private:
	message_history message_history_;
};
