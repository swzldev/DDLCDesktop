#include <behaviour/character_logic.h>

#include <string>
#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

#include <core/sys.h>
#include <core/window.h>
#include <core/input.h>
#include <behaviour/character_interaction.h>

using json = nlohmann::json;

character_logic::character_logic(window* window) {
	window_ = window;

	std::ifstream cfg("config.json");
	if (!cfg.is_open()) {
		throw std::runtime_error("Failed to open config.json (you will need to recreate it yourself or reinstall)");
	}

	json j;
	cfg >> j;

	std::string character_str = j.value("character", "monika");
	ddlc_character character = ddlc_character::MONIKA;
	if (character_str == "monika") {
		character = ddlc_character::MONIKA;
	}
	else if (character_str == "yuri") {
		character = ddlc_character::YURI;
	}
	else if (character_str == "natsuki") {
		character = ddlc_character::NATSUKI;
	}
	else if (character_str == "sayori") {
		character = ddlc_character::SAYORI;
	}
	else {
		throw std::runtime_error("Invalid character specified in config.json: '" + character_str + "'");
	}
	visuals = new character_visuals(window->get_renderer(), character);

	// set buttons
	visuals->add_text_button("Close", false, [this]() {
		close_button_click();
	});

	// create ai
	ai = new character_ai(character);

	if (std::filesystem::exists("character_state.json")) {
		ai->load_state("character_state.json");
	}

	window_->on_mouse_click.push_back([this]() {
		character_interaction interaction(character_interaction::kind::CLICK);
		handle_interaction(interaction);
		return 0;
	});

	// window opened
	character_interaction interaction(character_interaction::kind::WINDOW_OPEN);
	begin_think(interaction);
}
character_logic::~character_logic() {
	if (ai) {
		ai->save_state("character_state.json");
		delete ai;
		ai = nullptr;
	}
	if (visuals) {
		delete visuals;
		visuals = nullptr;
	}
}

void character_logic::handle_interaction(const character_interaction& interaction) {
	if (state_ == logic_state::IDLE) {
		begin_think(interaction);
	}
	else if (state_ == logic_state::TALKING && visuals->is_speaking()) {
		visuals->finish_speaking(); // mouse down while speaking -> quick skip
	}
	else if (state_ == logic_state::TALKING) {
		// in conversations, mouse down means advance interaction
		if (interaction_index_ < current_state.interactions.size()) {
			display_current_interaction();
			interaction_index_++;
		}
		else {
			// end of conversation
			interaction_index_ = 0;

			if (!current_state.actions.empty()) {
				await_choice();

				// (initial) custom button
				visuals->add_text_button("Custom", true, [this]() {
					custom_button_click();
				});
			}
			else {
				visuals->set_saying("");
				state_ = logic_state::IDLE;
			}
		}
	}
}

void character_logic::tick(float delta_time) {
	if (state_ == logic_state::THINKING) {
		if (!visuals->is_speaking()) {
			// constantly think
			display_think();
		}
		if (ai->is_response_ready()) {
			visuals->set_chars_per_second(50.0f);
			current_state = ai->get_response();
			if (!current_state.interactions.empty()) {
				state_ = logic_state::TALKING;
				interaction_index_ = 0;
				display_current_interaction();
				interaction_index_++;
			}
			else {
				state_ = logic_state::IDLE;
				visuals->set_saying("");
			}
		}
	}
	else if (state_ == logic_state::AWAITING_CHOICE) {
		int num_actions = current_state.actions.size();
		int choice = get_choice_input(num_actions);

		if (choice != -1) {
			// remove custom button (if applicable)
			if (custom_button_ != -1) {
				visuals->remove_text_button(custom_button_);
				custom_button_ = -1;
			}

			// user made a choice
			character_interaction choice_interaction(character_interaction::kind::CHOICE_MADE);
			choice_interaction.str_data = current_state.actions[choice];
			choice_interaction.int_data = choice + 1;

			begin_think(choice_interaction);
		}
	}
	else if (state_ == logic_state::AWAITING_INPUT) {
		// show current input
		std::string full = "You: " + current_input_ + "_";
		visuals->set_saying_immediate(full);
	}

	visuals->tick(delta_time);
}

void character_logic::close_button_click() {
	ai->handle_close_interaction();
	ai->save_state("character_state.json");

	window_->close();
}
void character_logic::custom_button_click() {
	await_input();

	// create actions button
	actions_button_ = visuals->add_text_button("Actions", true, [this]() {
		actions_button_ = -1;
		actions_button_click();
	});
}
void character_logic::actions_button_click() {
	await_choice(false);

	// create custom button
	custom_button_ = visuals->add_text_button("Custom", true, [this]() {
		custom_button_ = -1;
		custom_button_click();
	});
}

void character_logic::await_choice(bool show_immediate) {
	int num_actions = current_state.actions.size();
	std::string message = "Choose an action:\n";
	for (int i = 0; i < num_actions; i++) {
		message += std::to_string(i + 1) + ") " + current_state.actions[i] + "    ";
	}
	if (show_immediate) {
		visuals->set_saying_immediate(message);
	}
	else {
		visuals->set_saying(message);
	}

	state_ = logic_state::AWAITING_CHOICE;
}
void character_logic::await_input() {
	current_input_.clear();

	// start recording
	input::begin_input_recording(&current_input_, 50, [this]() {
		// remove actions button (if applicable)
		if (actions_button_ != -1) {
			visuals->remove_text_button(actions_button_);
			actions_button_ = -1;
		}

		// on submit
		character_interaction input_interaction(character_interaction::kind::CUSTOM_MESSAGE);
		input_interaction.str_data = current_input_;
		current_input_.clear();
		input::end_input_recording();
		begin_think(input_interaction);
	});

	state_ = logic_state::AWAITING_INPUT;
}

int character_logic::get_choice_input(int num_choices) {
	int choice = -1;

	if (input::is_key_pressed('1') && num_choices > 0) {
		choice = 0;
	}
	else if (input::is_key_pressed('2') && num_choices > 1) {
		choice = 1;
	}
	else if (input::is_key_pressed('3') && num_choices > 2) {
		choice = 2;
	}
	else if (input::is_key_pressed('4') && num_choices > 3) {
		choice = 3;
	}

	return choice;
}

void character_logic::begin_think(const character_interaction& interaction) {
	display_think();
	ai->handle_interaction_async(interaction);
	state_ = logic_state::THINKING;
}

void character_logic::display_think() {
	// slow effect while thinking
	visuals->set_chars_per_second(2.0f);
	visuals->set_saying("...");
}

void character_logic::display_current_interaction() {
	if (interaction_index_ < current_state.interactions.size()) {
		const auto& inter = current_state.interactions[interaction_index_];
		visuals->set_saying(inter.saying);

		visuals->set_expression(inter.expression);
		visuals->set_pose(inter.pose_left, inter.pose_right);

		const int screen_width = sys::display_width();
		const int screen_height = sys::display_height();

		int new_x = inter.new_x == -1 ? visuals->get_x() : inter.new_x;

		// pre-apply scale
		if (inter.new_scale != -1) {
			visuals->set_scale(inter.new_scale);
		}

		int scale = visuals->get_scale();

		// clamp position to screen bounds (or move if scale means position is out of bounds)
		// note x and y = top left (not centre)
		if (new_x < 0) {
			new_x = 0;
		}
		else if (new_x + scale > screen_width) {
			new_x = screen_width - scale;
		}

		// stick to the bottom of the screen
		int new_y = screen_height - scale;

		visuals->set_position(new_x, new_y);
	}
}
