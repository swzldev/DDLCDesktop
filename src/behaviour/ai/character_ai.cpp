#include <behaviour/ai/character_ai.h>

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <fstream>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include <behaviour/character_interaction.h>
#include <behaviour/ai/openai_api.h>
#include <behaviour/ai/system_prompts.h>
#include <output/log.h>

using json = nlohmann::json;

character_ai::character_ai(ddlc_character character) {
	character_ = character;

	std::ifstream config("config.json");
	if (!config.is_open()) {
		throw std::runtime_error("Failed to open config.json (you will need to recreate it yourself or reinstall)");
	}
	
	json j;
	config >> j;
	std::string api_key = j.value("openai_api_key", "");
	if (api_key.empty()) {
		throw std::runtime_error("OpenAI API key not found in config.json (did you read the installation instructions on the github?)");
	}

	model_ = j.value("openai_model", "gpt-4o-mini");
	message_history_size_ = j.value("openai_message_history_size", 6);
	user_name_ = j.value("user_name", "");
	system_prompt_ = j.value("behaviour_preset", "");

	openai_ = new openai_api(api_key);

	// initialize with system prompt
	add_to_history("system", get_system_prompt());
}
character_ai::~character_ai() {
	if (pending_response_.valid()) {
		pending_response_.wait();
	}

	delete openai_;
	openai_ = nullptr;
}

void character_ai::handle_close_interaction() {
	add_to_history("user", "[Interaction] " + get_user_name() + " closed Monika's window.");
}

void character_ai::handle_interaction_async(const character_interaction& interaction) {
	if (is_processing_) {
		return; // already processing
	}

	is_processing_ = true;

	pending_response_ = std::async(std::launch::async, [this, interaction]() {
		character_state state = handle_interaction_internal(interaction);
		is_processing_ = false;
		return state;
	});
}
bool character_ai::is_response_ready() const {
	if (!pending_response_.valid()) {
		return false;
	}

	return pending_response_.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}
character_state character_ai::get_response() {
	if (pending_response_.valid()) {
		return pending_response_.get();
	}
	return character_state{};
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

std::string character_ai::get_user_name() {
	if (user_name_.empty()) {
		return "The user";
	}
	return user_name_;
}

character_state character_ai::handle_interaction_internal(const character_interaction& interaction) {
	std::string prompt = build_prompt(interaction);
	std::string response = openai_->get_response(prompt);
	log::print("Generated prompt: {}\nAI Response: {}\n\n", prompt, response);

	std::string content = extract_content_from_response(response);

	// add to conversation history
	if (!content.empty()) {
		add_to_history("assistant", content);
	}

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
	switch (interaction.get_kind()) {
	case character_interaction::kind::CLICK:
		return "[Interaction] " + user_name + " clicked Monika.";
	case character_interaction::kind::CHOICE_MADE:
		return "[Interaction] " + user_name + ": \"" + interaction.str_data + "\" (Choice " + std::to_string(interaction.int_data) + ")";
	case character_interaction::kind::WINDOW_OPEN:
		return "[Interaction] " + user_name + " opened Monika's window.";
	default:
		return "[Interaction] " + user_name + " interacted with Monika, but an internal error occurred and we don't know what the interaction was.";
	}
}
std::string character_ai::extract_content_from_response(const std::string& response) {
	try {
		auto j = json::parse(response);

		// format: output[type == message] -> content[type == output_text] -> text
		json output_arr = j["output"];
		for (const auto& item : output_arr) {
			if (item["type"] == "message") {
				auto content_arr = item["content"];
				for (const auto& content_item : content_arr) {
					if (content_item["type"] == "output_text") {
						return content_item["text"].get<std::string>();
					}
				}
			}
		}
	}
	catch (...) {
		return "";
	}
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
			i.pose_left = get_pose_code(inter.value("pose_left", ""));
			i.pose_right = get_pose_code(inter.value("pose_right", ""));
			i.new_x = inter.value("new_x", -1); // default no change
			i.new_scale = inter.value("new_scale", -1); // default no change
			state.interactions.push_back(i);
		}

		// parse actions
		state.actions = j.value("actions", std::vector<std::string>{});

	}
	catch (...) {
		// fallback
		character_state::interaction i;
		i.saying = "";
		i.expression = "a";
		i.pose_left = "1";
		i.pose_right = "1";
		i.new_x = -1;
		i.new_scale = -1;
		state.interactions.push_back(i);
		state.actions = {};
	}

	return state;
}

std::string character_ai::get_pose_code(const std::string& pose) {
	static const std::unordered_map<std::string, std::string> monika_poses = {
		{"arm_at_side", "1"},
		{"casual_finger_point", "2"},
		{"hand_on_hip", "2"}
	};

	static const std::unordered_map<std::string, std::string> yuri_poses = {
		{"arm_behind_back", "1"},
		{"arm_fidget_at_chest", "2"}
	};

	const auto& pose_map = (character_ == ddlc_character::MONIKA)
		? monika_poses
		: monika_poses;

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
		{"eyes_dilated_open", "h2"},
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
		{"obsessive_eyes_joy", "y"},
		{"obsessive_nervous", "y2"},
		{"obsessive_manic_delight", "y3"},
		{"obsessive_ooh", "y4"},
		{"overjoyed", "y5"},
		{"soft_hesitation", "y6"},
		{"obsessive_angry", "y7"}
	};

	const auto& expr_map = (character_ == ddlc_character::MONIKA)
		? monika_expressions
		: yuri_expressions;

	auto it = expr_map.find(expression);
	if (it != expr_map.end()) {
		return it->second;
	}

	return "a"; // default fallback
}

std::string character_ai::get_system_prompt() const {
	return system_prompts::get_prompt(character_, system_prompt_);
}
