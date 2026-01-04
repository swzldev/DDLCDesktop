#pragma once

#include <string>
#include <vector>
#include <future>
#include <atomic>

#include <behaviour/character_state.h>
#include <behaviour/character_interaction.h>
#include <behaviour/ai/openai_api.h>

class character_ai {
public:
	static constexpr auto MODEL_NAME = "gpt-4o-mini";
	static constexpr int MAX_HISTORY_MESSAGES = 6;

public:
	character_ai();
	~character_ai();

	void handle_interaction_async(const character_interaction& interaction);
	bool is_response_ready() const;
	character_state get_response();

private:
	openai_api* openai_;

	struct message {
		std::string role;
		std::string content;
	};
	std::vector<message> conversation_history_;

	std::future<character_state> pending_response_;
	std::atomic<bool> is_processing_ = false;

	inline void add_to_history(const std::string& role, const std::string& content) {
		conversation_history_.push_back({ role, content });
		// limit history size (dont include system prompt)
		if (conversation_history_.size() > MAX_HISTORY_MESSAGES + 1) {
			conversation_history_.erase(conversation_history_.begin());
		}
	}

	character_state handle_interaction_internal(const character_interaction& interaction);
	std::string build_prompt(const character_interaction& interaction);
	std::string interaction_to_message(const character_interaction& interaction);
	std::string extract_content_from_response(const std::string& response);
	character_state parse_response(const std::string& raw_response);

	static std::string get_static_prompt();
};