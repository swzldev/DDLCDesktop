#pragma once

#include <string>
#include <memory>

#include <ddlc/characters.h>

enum class api {
	OPENAI,
	OPENROUTER,
};

class config {
public:
	static config* load();

	api api;
	std::string api_key;
	std::string model;
	int message_history_size;

	std::string user_name;
	std::string behaviour_preset;
	ddlc_character character;

private:
	static std::unique_ptr<config> loaded_;
};