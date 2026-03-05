#pragma once

#include <vector>
#include <string>

#include <ai/ai_message.h>

class imessage_repository {
public:
	virtual ~imessage_repository() = default;

	virtual std::vector<ai_message> get_messages(int channel) = 0;
	virtual void save_message(int channel, const std::string& role, const std::string& content) = 0;
};
