#include <behaviour/memory/message_history.h>

void message_history::add_message(const std::string& role, const std::string& content) {
	history_.push_back({ role, content });
}

std::vector<message_entry> message_history::get_history(int limit) const {
	if (limit < 0 || limit >= static_cast<int>(history_.size())) {
        return history_;
    }
    else {
        return std::vector<message_entry>(history_.end() - limit, history_.end());
    }
}
