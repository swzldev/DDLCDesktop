#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include <ddlc/characters.h>
#include <behaviour/character_state.h>
#include <behaviour/character_interaction.h>
#include <behaviour/ai/ai_api.h>

#include <nlohmann/json.hpp>

class character_ai {
public:
	character_ai(ddlc_character character);
	~character_ai();

	void handle_close_interaction(); // doesnt use AI, so dont need async

	void handle_interaction_async(const character_interaction& interaction);
	bool is_response_ready() const;
	character_state get_response();

	void save_state(const char* path);
	void load_state(const char* path);
	void reset_state();
	void cancel_and_reset();

	std::string get_user_name() const;
	std::string get_character_name() const;

	std::string now_str() const;

private:
	std::string endpoint_;
	std::string api_key_;
	ai_api* api_;
	std::string model_ = "gpt-4o-mini";
	int message_history_size_;
	std::string user_name_ = "";
	std::string system_prompt_ = "";
	ddlc_character character_;

	struct message {
		std::string role;
		std::string content;
	};
	std::vector<message> conversation_history_;

	std::thread worker_;
	std::atomic<bool> stop_worker_{ false };
	std::atomic<bool> has_task_{ false };
	std::atomic<bool> is_processing_{ false };
	std::atomic<bool> has_result_{ false };
	std::atomic<bool> cancel_requested_{ false };
	character_interaction pending_interaction_;
	character_state pending_result_;
	mutable std::mutex mutex_;
	std::condition_variable cv_;

	void request_cancel();

	void load_config(nlohmann::json j);

	inline void add_to_history(const std::string& role, const std::string& content) {
		conversation_history_.push_back({ role, content });
		// limit history size (dont include system prompt)
		if (conversation_history_.size() > message_history_size_ + 1) {
			conversation_history_.erase(conversation_history_.begin() + 1);
		}
	}

	void worker_loop();

	character_state handle_interaction_internal(const character_interaction& interaction);
	std::string build_prompt(const character_interaction& interaction);
	std::string interaction_to_message(const character_interaction& interaction);
	std::string extract_content_from_response(const std::string& response);
	character_state parse_response(const std::string& raw_response);
	std::string get_pose_code(const std::string& pose);
	std::string get_expression_code(const std::string& expression);

	std::string get_system_prompt() const;
};