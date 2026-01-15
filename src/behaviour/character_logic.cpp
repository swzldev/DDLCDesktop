#include <behaviour/character_logic.h>

#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include <core/sys.h>
#include <core/window.h>
#include <core/input.h>
#include <behaviour/character_interaction.h>
#include <behaviour/ai/character_ai.h>
#include <behaviour/character_state.h>
#include <ddlc/characters.h>
#include <visual/character_visuals.h>
#include <visual/ui/button.h>
#include <error/ddlcd_runtime_error.h>
#include <error/error_stories.h>
#include <config/config.h>

using json = nlohmann::json;
namespace fs = std::filesystem;

character_logic::character_logic(window* window) {
	window_ = window;

	if (config::load()) {
		character_ = config::get()->character;
	}

	// create visuals
	visuals = new character_visuals(window_->get_renderer(), character_);

	// main menu
	show_main_menu();

	// create ai
	ai = new character_ai(character_);

	if (fs::exists("character_state.json")) {
		ai->load_state("character_state.json");
	}

	window_->on_mouse_click.push_back([this]() {
		character_interaction interaction(character_interaction::kind::CLICK);
		handle_interaction(interaction);
		return 0;
	});
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
	if (interaction.get_kind() != character_interaction::kind::CLICK || current_menu_ != menu_state::MAIN) {
		return;
	}

	if (state_ == logic_state::IDLE) {
		begin_think(interaction);
	}
	else if (state_ == logic_state::TALKING && visuals->is_speaking()) {
		visuals->finish_speaking(); // mouse down while speaking -> quick skip
	}
	else if (state_ == logic_state::TALKING) {
		// in conversations, mouse down means advance interaction
		if (++interaction_index_ < current_state.interactions.size()) {
			display_current_interaction();
		}
		else {
			// end of conversation
			interaction_index_ = 0;

			if (error_state_ == error_state::NON_FATAL) {
				reset_all();
			}
			else if (error_state_ == error_state::FATAL) {
				window_->close();
			}
			else {
				if (!current_state.actions.empty()) {
					if (custom_mode_) {
						await_input();
					}
					else {
						await_choice();
					}
					input_mode_btn_disabled_ = false;
				}
				else {
					state_ = logic_state::IDLE;
					refresh_display();
				}
			}
		}
	}
}

void character_logic::tick(float delta_time) {
	if (first_tick_) {
		if (config::load()) {
			// window opened
			character_interaction interaction(character_interaction::kind::WINDOW_OPEN);
			begin_think(interaction);
		}
		else {
			handle_error(ddlcd_runtime_error(ddlcd_error::FAIL_OPEN_CONFIG, "Failed to open config.json"));
		}

		first_tick_ = false;
	}

	if (current_menu_ != menu_state::MAIN) {
		visuals->tick(delta_time);
		return;
	}

	if (state_ == logic_state::THINKING) {
		if (!visuals->is_speaking()) {
			// constantly think
			display_think();
		}
		if (ai->is_response_ready()) {
			visuals->set_chars_per_second(50.0f);
			current_state = ai->get_response();
			if (character_ == ddlc_character::MONIKA) {
				visuals->set_style("normal"); // monika doesnt have casual style
			}
			else visuals->set_style(current_state.style);
			
			// handle errors
			if (current_state.err != character_state::error::NONE) {
				switch (current_state.err) {
				case character_state::error::FAIL_PARSE_RESPONSE_JSON:
					handle_error(ddlcd_runtime_error(ddlcd_error::FAIL_PARSE_AI_RESPONSE, "Failed to parse AI response as JSON."));
					return;
				case character_state::error::FAIL_PARSE_RESPONSE_UNKNOWN:
					handle_error(ddlcd_runtime_error(ddlcd_error::FAIL_PARSE_AI_RESPONSE, "Failed to parse AI response due to unknown format."));
					return;
				default:
					break;
				}
			}

			if (!current_state.interactions.empty()) {
				state_ = logic_state::TALKING;
				interaction_index_ = 0;
				display_current_interaction();
			}
			else {
				state_ = logic_state::IDLE;
				refresh_display();
			}
		}
	}
	else if (state_ == logic_state::AWAITING_CHOICE) {
		int num_actions = current_state.actions.size();
		int choice = get_choice_input(num_actions);

		if (choice != -1) {
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

void character_logic::handle_error(const ddlcd_runtime_error& error) {
	// cleanup
	current_state.interactions.clear();

	const int size = 1000;
	int x = (sys::display_width() / 2) - (size / 2);
	int y = (sys::display_height()) - size;

	visuals->set_position(x, y);
	visuals->set_scale(size);

	visuals->set_expression("a");
	visuals->set_pose("1", "1");
	visuals->set_character(ddlc_character::MONIKA);
	visuals->set_chars_per_second(50.0f);
	
	bool fatal = false;

	switch (error.kind) {
	case ddlcd_error::FAIL_OPEN_CONFIG:
		current_state.interactions = error_stories::fail_load_config_story();
		fatal = true;
		break;
	case ddlcd_error::FAIL_AI_RESPONSE:
		current_state.interactions = error_stories::fail_ai_response_story();
		fatal = true;
		break;
	case ddlcd_error::FAIL_PARSE_AI_RESPONSE:
		current_state.interactions = error_stories::fail_parse_ai_response_story();
		break;
	case ddlcd_error::OTHER:
		current_state.interactions.push_back({ "Uh oh! An error has occurred: " + error.message, "h", "1", "1"});
		break;
	}

	if (!fatal) {
		current_state.interactions.push_back({ "The application will now reset.", "a", "1", "1" });
	}

	error_state_ = fatal ? error_state::FATAL : error_state::NON_FATAL;
	state_ = logic_state::TALKING;

	interaction_index_ = 0;
	display_current_interaction();
}

void character_logic::show_main_menu() {
	visuals->clear_buttons();
	current_menu_ = menu_state::MAIN;

	// set buttons
	visuals->add_button({ "Close", [this]() {
		ai->handle_close_interaction();
		ai->save_state("character_state.json");

		window_->close();
	} });
	visuals->add_button({ "Reset", [this]() {
		if (state_ == logic_state::THINKING) {
			return; // dont reset while thinking
		}
		reset_all();
	} });
	visuals->add_button({ "Custom", [this]() {
			await_input();
		},
		button_style::LABEL, button_type::TOGGLE,
		"Actions", [this]() {
			await_choice(/*true*/);
		},
		&input_mode_btn_disabled_
	});
	visuals->add_button({ "Settings", [this]() {
		show_settings_menu();
	} });

	refresh_display();
}
void character_logic::show_settings_menu() {
	visuals->clear_buttons();
	current_menu_ = menu_state::SETTINGS;

	visuals->set_chars_per_second(100.0f);
	visuals->set_saying("Choose an option...");

	// set buttons
	visuals->add_button({ "Character", [this]() {
		
	} });
	visuals->add_button({ "Back", [this]() {
		show_main_menu();
	} });
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

	custom_mode_ = false;
	state_ = logic_state::AWAITING_CHOICE;
}
void character_logic::await_input() {
	current_input_.clear();

	// start recording
	input::begin_input_recording(&current_input_, INPUT_MAX_LENGTH, [this]() {
		// on submit
		character_interaction input_interaction(character_interaction::kind::CUSTOM_MESSAGE);
		input_interaction.str_data = current_input_;
		current_input_.clear();
		input::end_input_recording();
		begin_think(input_interaction);
	});

	custom_mode_ = true;
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

void character_logic::reset_all() {
	// cleanup
	if (fs::exists("character_state.json")) {
		fs::remove("character_state.json");
	}

	ai->cancel_and_reset();
	current_state.interactions.clear();
	interaction_index_ = 0;
	error_state_ = error_state::NONE;

	visuals->reset(character_);
	show_main_menu();

	// start new interaction
	character_interaction interaction(character_interaction::kind::WINDOW_OPEN);
	begin_think(interaction);
}

void character_logic::begin_think(const character_interaction& interaction) {
	input_mode_btn_disabled_ = true;
	display_think();
	ai->handle_interaction_async(interaction);
	state_ = logic_state::THINKING;
}

void character_logic::display_think() {
	// slow effect while thinking
	visuals->set_chars_per_second(2.0f);
	visuals->set_saying("... ");
}

void character_logic::display_current_interaction() {
	if (interaction_index_ < current_state.interactions.size()) {
		const auto& inter = current_state.interactions[interaction_index_];
		visuals->set_saying(inter.saying);

		visuals->set_expression(inter.expression);
		visuals->set_pose(inter.pose_left, inter.pose_right);

		const int screen_width = sys::display_width();
		const int screen_height = sys::display_height();

		int min_height = screen_height * 0.5f;
		int max_height = screen_height * 0.95f;

		int new_x_px;
		if (inter.new_x == -1) {
			new_x_px = visuals->get_x();
		}
		else {
			int new_x = inter.new_x;
			if (new_x < 0) new_x = 0;
			else if (new_x > 100) new_x = 100;

			new_x_px = static_cast<int>((new_x / 100.0f) * screen_width);
		}

		// pre-apply scale
		if (inter.new_scale != -1) {
			int new_scale = inter.new_scale;
			if (new_scale > 10) new_scale = 10;
			else if (new_scale < 1) new_scale = 1;

			// scale from 1-10 to min_height-max_height
			float scale_factor = (new_scale - 1) / 9.0f; // normalize to 0.0-1.0
			new_scale = min_height + static_cast<int>(scale_factor * (max_height - min_height));

			visuals->set_scale(new_scale);
		}

		int scale = visuals->get_scale();

		// clamp position to screen bounds
		if (new_x_px < 0) new_x_px = 0;
		else if (new_x_px + scale > screen_width) {
			new_x_px = screen_width - scale;
		}

		// stick to the bottom of the screen
		int new_y = screen_height - scale;

		visuals->set_position(new_x_px, new_y);
	}
}

void character_logic::refresh_display() {
	switch (state_) {
	case logic_state::IDLE:
		visuals->set_chars_per_second(50.0f);
		visuals->set_saying("");
		input_mode_btn_disabled_ = true;
		break;

	case logic_state::THINKING:
		display_think();
		input_mode_btn_disabled_ = true;
		break;

	case logic_state::TALKING:
		visuals->set_chars_per_second(50.0f);
		display_current_interaction();
		input_mode_btn_disabled_ = true;
		break;

	case logic_state::AWAITING_CHOICE:
		await_choice(true);
		input_mode_btn_disabled_ = false;
		break;

	case logic_state::AWAITING_INPUT:
		// restore input mode
		custom_mode_ = true;
		input_mode_btn_disabled_ = false;

		// restart input recording
		input::begin_input_recording(&current_input_, INPUT_MAX_LENGTH, [this]() {
			character_interaction input_interaction(character_interaction::kind::CUSTOM_MESSAGE);
			input_interaction.str_data = current_input_;
			current_input_.clear();
			input::end_input_recording();
			begin_think(input_interaction);
		});

		std::string full = "You: " + current_input_ + "_";
		visuals->set_saying_immediate(full);
		break;
	}
}
