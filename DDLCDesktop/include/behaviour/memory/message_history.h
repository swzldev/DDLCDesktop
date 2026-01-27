#pragma once

#include <string>
#include <vector>

struct message_entry {
	std::string role;
	std::string content;
};

class message_history {
public:
	message_history() = default;
	
	void add_message(const std::string& role, const std::string& content);
	std::vector<message_entry> get_history(int limit = -1) const;

private:
	std::vector<message_entry> history_;
};
