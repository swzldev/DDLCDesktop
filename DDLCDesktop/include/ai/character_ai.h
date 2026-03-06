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
#include <persistance/imessage_repository.h>
#include <ai/ai_api.h>
#include <config/config.h>

#include <nlohmann/json.hpp>

class character_ai {
public:
	character_ai();
	~character_ai();

	void handle_close_interaction(); // doesnt use AI, so dont need async

	void handle_interaction_async(const character_interaction& interaction);
	bool is_response_ready() const;
	character_state get_response();

	inline void set_repository(imessage_repository* repo) {
		repo_ = repo;
	}
	void reset_state();
	void cancel();
	void cancel_and_reset();

	std::string get_user_name() const;

	std::string now_str() const;

private:
	config* config_ = nullptr;
	imessage_repository* repo_ = nullptr;
	ai_api* api_;

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

	inline int get_cur_channel() {
		return (int)config_->character;
	}
	inline void add_user_msg(const std::string& msg) {
		if (!repo_) return;
		repo_->save_message("user", msg, get_cur_channel());
	}
	inline void add_ai_msg(const std::string& msg) {
		if (!repo_) return;
		repo_->save_message("assistant", msg, get_cur_channel());
	}

	void request_cancel();

	void worker_loop();

	std::string get_endpoint() const;

	character_state handle_interaction_internal(const character_interaction& interaction);
	std::string build_prompt(const character_interaction& interaction);
	std::string interaction_to_message(const character_interaction& interaction);
	std::string extract_json(const std::string& str);
	bool response_is_error(const std::string& response);
	std::string extract_content_from_response(const std::string& response);
	character_state parse_response(const std::string& raw_response);
	std::string get_pose_code_left(const std::string& pose);
	std::string get_pose_code_right(const std::string& pose);
	std::string get_expression_code(const std::string& expression);

	std::string get_system_prompt() const;
};