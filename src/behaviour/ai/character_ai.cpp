#include <behaviour/ai/character_ai.h>

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <format>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include <nlohmann/json.hpp>

#include <behaviour/character_interaction.h>
#include <behaviour/ai/ai_api.h>
#include <behaviour/ai/system_prompts.h>
#include <output/log.h>
#include <error/ddlcd_runtime_error.h>

using json = nlohmann::json;

character_ai::character_ai(ddlc_character character) {
	character_ = character;

	std::ifstream config("config.json");
	if (config.is_open()) {
		json j;
		config >> j;
		config.close();

		load_config(j);

		api_ = new ai_api(endpoint_, api_key_);
	}
	
	// initialize with system prompt
	add_to_history("system", get_system_prompt());

	// start worker thread
	stop_worker_.store(false, std::memory_order_relaxed);
	worker_ = std::thread(&character_ai::worker_loop, this);
}
character_ai::~character_ai() {
	request_cancel();

	{
		std::lock_guard<std::mutex> lock(mutex_);
		stop_worker_.store(true, std::memory_order_relaxed);
	}
	cv_.notify_all();

	if (worker_.joinable()) {
		worker_.join();
	}

	delete api_;
	api_ = nullptr;
}

void character_ai::handle_close_interaction() {
	add_to_history("user", "[" + now_str() + "] " + get_user_name() + " closed " + get_character_name() + "'s window.");
}

void character_ai::handle_interaction_async(const character_interaction& interaction) {
	if (has_task_.load(std::memory_order_relaxed) || is_processing_.load(std::memory_order_relaxed)) {
		return;
	}

	{
		std::lock_guard<std::mutex> lock(mutex_);
		pending_interaction_ = interaction;
		has_task_.store(true, std::memory_order_relaxed);
		has_result_.store(false, std::memory_order_relaxed);
		cancel_requested_.store(false, std::memory_order_relaxed);
	}
	cv_.notify_one();
}
bool character_ai::is_response_ready() const {
	return has_result_.load(std::memory_order_relaxed);
}
character_state character_ai::get_response() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (!has_result_) {
		return character_state{};
	}

	has_result_.store(false, std::memory_order_relaxed);
	is_processing_.store(false, std::memory_order_relaxed);
	return pending_result_;
}

void character_ai::save_state(const char* path) {
	std::ofstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file for saving character AI state");
	}

	json j;
	j["conversation_history"] = json::array();
	for (const auto& msg : conversation_history_) {
		j["conversation_history"].push_back({ {"role", msg.role}, {"content", msg.content} });
	}
	file << j.dump(4);

	file.close();
}
void character_ai::load_state(const char* path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		return; // no saved state
	}

	json j;
	file >> j;

	conversation_history_.clear();
	for (const auto& msg : j["conversation_history"]) {
		conversation_history_.push_back({ msg["role"].get<std::string>(), msg["content"].get<std::string>() });
	}

	file.close();
}

void character_ai::reset_state() {
	conversation_history_.clear();
	add_to_history("system", get_system_prompt());
}
void character_ai::cancel_and_reset() {
	request_cancel();
	{
		std::lock_guard<std::mutex> lock(mutex_);
		has_task_.store(false, std::memory_order_relaxed);
		has_result_.store(false, std::memory_order_relaxed);
		is_processing_.store(false, std::memory_order_relaxed);
	}
	reset_state();
}

std::string character_ai::get_user_name() const {
	if (user_name_.empty()) {
		return "The user";
	}
	return user_name_;
}
std::string character_ai::get_character_name() const {
	switch (character_) {
	case ddlc_character::MONIKA:
		return "Monika";
	case ddlc_character::YURI:
		return "Yuri";
	case ddlc_character::NATSUKI:
		return "Natsuki";
	case ddlc_character::SAYORI:
		return "Sayori";
	default:
		return "Character"; // fallback
	}
}

std::string character_ai::now_str() const {
	auto now = std::chrono::system_clock::now();
	return std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::floor<std::chrono::seconds>(now));
}

void character_ai::request_cancel() {
	cancel_requested_.store(true, std::memory_order_relaxed);

	if (api_) {
		api_->cancel();
	}
}

void character_ai::load_config(nlohmann::json j) {
	// API config
	std::string api = j.value("api", "openai");
	if (api == "openai") {
		endpoint_ = "https://api.openai.com/v1/responses";
	}
	else if (api == "openrouter") {
		endpoint_ = "https://openrouter.ai/api/v1/responses";
	}
	else {
		throw std::runtime_error("Unsupported API specified in config.json: '" + api + "'");
	}

	api_key_ = j.value("api_key", "");
	if (api_key_.empty()) {
		log::print("API key not found in config.json, throwing...\nconfig.json contents:\n{}\n", j.dump(4));
		throw std::runtime_error("API key not found in config.json (did you read the installation instructions on the github?)");
	}

	if (api == "openai") {
		model_ = j.value("model", "gpt-4o-mini");
	}
	else if (api == "openrouter") {
		model_ = j.value("model", "openrouter/openai/gpt-4o-mini");
	}
	message_history_size_ = j.value("message_history_size", 6);

	// other config
	user_name_ = j.value("user_name", "");
	system_prompt_ = j.value("behaviour_preset", "");
}

void character_ai::worker_loop() {
	while (true) {
		character_interaction interaction;

		// wait for work or stop
		{
			std::unique_lock<std::mutex> lock(mutex_);
			cv_.wait(lock, [this]() {
				return stop_worker_.load(std::memory_order_relaxed)
					|| has_task_.load(std::memory_order_relaxed);
				});
			if (stop_worker_) {
				is_processing_.store(false, std::memory_order_relaxed);
				return;
			}

			interaction = pending_interaction_;
			has_task_.store(false, std::memory_order_relaxed);
			is_processing_.store(true, std::memory_order_relaxed);
		}

		if (cancel_requested_.load(std::memory_order_relaxed)) {
			is_processing_.store(false, std::memory_order_relaxed);
			continue;
		}

		if (!api_) {
			// no API configured; treat as empty response
			character_state empty_state{};
			{
				std::lock_guard<std::mutex> lock(mutex_);
				pending_result_ = empty_state;
				has_result_.store(true, std::memory_order_relaxed);
				is_processing_.store(false, std::memory_order_relaxed);
			}
			continue;
		}

		character_state result{};
		try {
			result = handle_interaction_internal(interaction);
		}
		catch (...) {
			{
				std::lock_guard<std::mutex> lock(mutex_);
				pending_result_ = result;
				has_result_.store(true, std::memory_order_relaxed);
				is_processing_.store(false, std::memory_order_relaxed);
			}
			continue;
		}

		if (cancel_requested_.load(std::memory_order_relaxed)) {
			continue;
		}

		// store result
		{
			std::lock_guard<std::mutex> lock(mutex_);
			pending_result_ = result;
			has_result_.store(true, std::memory_order_relaxed);
			is_processing_.store(false, std::memory_order_relaxed);
		}
	}
}

character_state character_ai::handle_interaction_internal(const character_interaction& interaction) {
	std::string prompt = build_prompt(interaction);
	std::string response = api_->get_response(prompt);

#if defined(DEBUG) || defined(_DEBUG)
	log::print("Generated prompt: {}\nAI Response: {}\n\n", prompt, response);
#endif

	character_state fail_parse_state{};
	fail_parse_state.err = character_state::error::FAIL_PARSE_RESPONSE_UNKNOWN;

	if (response_is_error(response)) {
		return fail_parse_state;
	}

	std::string content = extract_content_from_response(response);

	// add to conversation history
	if (!content.empty()) {
		add_to_history("assistant", content);
	}
	else return fail_parse_state;

	return parse_response(content);
}

std::string character_ai::build_prompt(const character_interaction& interaction) {
	json messages = json::array();

	// add conversation history
	for (const auto& msg : conversation_history_) {
		messages.push_back({ {"role", msg.role}, {"content", msg.content} });
	}

	std::string interaction_message = interaction_to_message(interaction);
	messages.push_back({ {"role", "user"}, {"content", interaction_message} });

	// add to history
	add_to_history("user", interaction_message);

	json payload;
	payload["model"] = model_;
	payload["input"] = messages;

	return payload.dump();
}
std::string character_ai::interaction_to_message(const character_interaction& interaction) {
	std::string user_name = get_user_name();
	std::string character_name = get_character_name();
	switch (interaction.get_kind()) {
	case character_interaction::kind::CLICK:
		return "[" + now_str() + "] " + user_name + " clicked " + character_name + ".";
	case character_interaction::kind::CHOICE_MADE:
		return "[" + now_str() + "] " + user_name + ": \"" + interaction.str_data + "\" (Choice " + std::to_string(interaction.int_data) + ")";
	case character_interaction::kind::CUSTOM_MESSAGE:
		return "[" + now_str() + "] " + user_name + ": \"" + interaction.str_data + "\"";
	case character_interaction::kind::WINDOW_OPEN:
		return "[" + now_str() + "] " + user_name + " opened " + character_name + "'s window.";
	default:
		return "[" + now_str() + "] " + user_name + " interacted with " + character_name + ", but an internal error occurred and we don't know what the interaction was.";
	}
}
bool character_ai::response_is_error(const std::string& response) {
	try {
		auto j = json::parse(response);

		if (j.contains("error") && !j["error"].is_null()) {
			log::print("Response was an error: {}\n", j["error"].dump());
			return true;
		}
	}
	catch (...) {
		return true;
	}
	return false;
}
std::string character_ai::extract_content_from_response(const std::string& response) {
	try {
		auto j = json::parse(response);

		if (j.contains("error") && !j["error"].is_null()) {
			return "";
		}

		// format: output[type == message] -> content[type == output_text] -> text
		json output_arr = j["output"];
		for (const auto& item : output_arr) {
			if (item["type"] == "message") {
				json content_arr = item["content"];
				for (const auto& content_item : content_arr) {
					if (content_item["type"] == "output_text") {
						return content_item["text"].get<std::string>();
					}
				}
			}
		}
	}
	catch (...) {
	}
	return "";
}
character_state character_ai::parse_response(const std::string& raw_response) {
	character_state state{};

	try {
		auto j = json::parse(raw_response);

		// parse interactions
		for (auto& inter : j["interactions"]) {
			character_state::interaction i;
			i.saying = inter.value("saying", "");
			i.expression = get_expression_code(inter.value("expression", ""));
			i.pose_left = get_pose_code_left(inter.value("pose_left", ""));
			i.pose_right = get_pose_code_right(inter.value("pose_right", ""));
			i.new_x = inter.value("new_x", -1); // default no change
			i.new_scale = inter.value("new_scale", -1); // default no change
			state.interactions.push_back(i);
		}

		// parse style + actions
		state.style = j.value("style", "normal");
		if (state.style != "normal" && state.style != "casual") {
			state.style = "normal"; // fallback
		}

		state.actions = j.value("actions", std::vector<std::string>{});
	}
	catch (nlohmann::json::exception& e) {
		log::print("JSON parsing error in AI response: {}\n", e.what());

		state.err = character_state::error::FAIL_PARSE_RESPONSE_JSON;
	}
	catch (...) {
		state.err = character_state::error::FAIL_PARSE_RESPONSE_UNKNOWN;
	}

	return state;
}

std::string character_ai::get_pose_code_left(const std::string& pose) {
	static const std::unordered_map<std::string, std::string> monika_poses = {
		{"neutral", "1"},
		{"one_hand_hip", "1"},
		{"one_hand_finger_point", "2"},
		{"hip_point", "2"}
	};

	static const std::unordered_map<std::string, std::string> yuri_poses = {
		{"neutral", "1"},
		{"one_arm_at_chest", "1"},
		{"chest_hand_fidget", "2"}
	};

	static const std::unordered_map<std::string, std::string> natsuki_poses = {
		{"neutral", "1"},
		{"left_hand_hip", "2"},
		{"right_hand_hip", "1"},
		{"hands_on_hips", "2"}
	};

	static const std::unordered_map<std::string, std::string> sayori_poses = {
		{"neutral", "1"},
		{"left_arm_raised", "2"},
		{"right_arm_raised", "1"},
		{"arms_raised", "2"},
	};

	const auto& pose_map = (character_ == ddlc_character::MONIKA)
		? monika_poses
		: (character_ == ddlc_character::YURI)
		? yuri_poses
		: (character_ == ddlc_character::NATSUKI)
		? natsuki_poses
		: (character_ == ddlc_character::SAYORI)
		? sayori_poses
		: throw std::runtime_error("Unknown character when getting pose code");

	auto it = pose_map.find(pose);
	if (it != pose_map.end()) {
		return it->second;
	}

	return "1"; // default fallback
}
std::string character_ai::get_pose_code_right(const std::string& pose) {
	static const std::unordered_map<std::string, std::string> monika_poses = {
		{"neutral", "1"},
		{"one_hand_hip", "2"},
		{"one_hand_finger_point", "1"},
		{"hip_point", "2"}
	};

	static const std::unordered_map<std::string, std::string> yuri_poses = {
		{"neutral", "1"},
		{"one_arm_at_chest", "2"},
		{"chest_hand_fidget", "2"}
	};

	static const std::unordered_map<std::string, std::string> natsuki_poses = {
		{"neutral", "1"},
		{"left_hand_hip", "1"},
		{"right_hand_hip", "2"},
		{"hands_on_hips", "2"}
	};

	static const std::unordered_map<std::string, std::string> sayori_poses = {
		{"neutral", "1"},
		{"left_arm_raised", "1"},
		{"right_arm_raised", "2"},
		{"arms_raised", "2"},
	};

	const auto& pose_map = (character_ == ddlc_character::MONIKA)
		? monika_poses
		: (character_ == ddlc_character::YURI)
		? yuri_poses
		: (character_ == ddlc_character::NATSUKI)
		? natsuki_poses
		: (character_ == ddlc_character::SAYORI)
		? sayori_poses
		: throw std::runtime_error("Unknown character when getting pose code");

	auto it = pose_map.find(pose);
	if (it != pose_map.end()) {
		return it->second;
	}

	return "1"; // default fallback
}
std::string character_ai::get_expression_code(const std::string& expression) {
	static const std::unordered_map<std::string, std::string> monika_expressions = {
		{"smile", "a"},
		{"open_smile", "b"},
		{"neutral", "c"},
		{"neutral_open", "d"},
		{"soft_smile", "e"},
		{"neutral_raised", "f"},
		{"neutral_open_raised", "g"},
		{"serious", "h"},
		{"serious_open", "i"},
		{"warm_smile", "j"},
		{"joyful", "k"},
		{"nervous_laugh", "l"},
		{"nervous_smile", "m"},
		{"awkward_laugh", "n"},
		{"nervous", "o"},
		{"nervous_open", "p"},
		{"relaxed", "q"},
		{"open_relaxed", "r"}
	};

	static const std::unordered_map<std::string, std::string> yuri_expressions = {
		{"smile", "a"},
		{"head_tilt_look_forward", "a2"},
		{"open_smile", "b"},
		{"head_tilt_look_away", "b2"},
		{"warm_smile", "c"},
		{"head_tilt_face_red", "c2"},
		{"joyful", "d"},
		{"head_tilt_nervous", "d2"},
		{"ooh", "e"},
		{"head_tilt_face_red_smile", "e2"},
		{"amazed", "f"},
		{"serious", "g"},
		{"serious_open_slightly", "h"},
		{"eyes_dilated_open", "hisui"},
		{"serious_smile", "i"},
		{"serious_open_wide", "j"},
		{"breathe_eyes_closed", "k"},
		{"breathe_eyes_closed_2", "l"},
		{"smile_eyes_closed", "m"},
		{"nervous", "n"},
		{"nervous_look_away", "o"},
		{"distraught", "p"},
		{"nervous_laugh", "q"},
		{"angry_serious", "r"},
		{"relieved", "s"},
		{"nervous_breath", "t"},
		{"soft_smile", "u"},
		{"hesitant_worry", "v"},
		{"relieved_breath", "w"},
		{"obsessive_eyes_joy", "y1"},
		{"obsessive_nervous", "y2"},
		{"obsessive_manic_delight", "y3"},
		{"obsessive_ooh", "y4"},
		{"overjoyed", "y5"},
		{"soft_hesitation", "y6"},
		{"obsessive_angry", "y7"}
	};

	static const std::unordered_map<std::string, std::string> natsuki_expressions = {
		{"smile", "a"},
		{"talk", "b"},
		{"mouth_open", "c"},
		{"smile_open", "d"},
		{"talk_serious", "e"},
		{"frustrated", "f"},
		{"annoyed", "g"},
		{"flustered_open", "h"},
		{"flustered", "i"},
		{"smile_sincere", "j"},
		{"talk_sincere", "k"},
		{"joyful", "l"},
		{"concerned_open", "m"},
		{"concerned", "n"},
		{"angry", "o"},
		{"shout", "p"},
		{"look_away_open", "q"},
		{"look_away_angry", "r"},
		{"look_away", "s"},
		{"scream", "scream"},
		{"cool_smile_open", "t"},
		{"concerned_look_away", "u"},
		{"cute_moan", "v"},
		{"talk_eyes_closed", "w"},
		{"angry_eyes_closed", "x"},
		{"smartass", "y"},
		{"overjoyed", "z"}
	};

	static const std::unordered_map<std::string, std::string> sayori_expressions = {
		{"smile", "a"},
		{"neutral", "b"},
		{"talk", "c"},
		{"smile_sincere", "d"},
		{"hesitant", "e"},
		{"concern", "f"},
		{"concern_open", "g"},
		{"concern_talk", "h"},
		{"serious", "i"},
		{"serious_talk", "j"},
		{"soft_sorrow", "k"},
		{"nervous_laugh", "l"},
		{"surprised", "m"},
		{"amazed_ooh", "n"},
		{"serious_nervous", "o"},
		{"cute_moan", "p"},
		{"warm_smile", "q"},
		{"joyful", "r"},
		{"joyful_blush", "s"},
		{"sincere_cry", "t"},
		{"sad_cry", "u"},
		{"cry_blush", "v"},
		{"cry", "w"},
		{"happy", "x"},
		{"smile_sincere_blush", "y"}
	};

	const auto& expr_map = (character_ == ddlc_character::MONIKA)
		? monika_expressions
		: (character_ == ddlc_character::YURI)
		? yuri_expressions
		: (character_ == ddlc_character::NATSUKI)
		? natsuki_expressions
		: (character_ == ddlc_character::SAYORI)
		? sayori_expressions
		: throw std::runtime_error("Unknown character when getting expression code");

	auto it = expr_map.find(expression);
	if (it != expr_map.end()) {
		return it->second;
	}

	return "a"; // default fallback
}

std::string character_ai::get_system_prompt() const {
	return system_prompts::get_prompt(character_, system_prompt_);
}
