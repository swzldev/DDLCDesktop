#include <behaviour/character_logic.h>

#include <string>
#include <filesystem>

#include <core/input.h>
#include <behaviour/character_interaction.h>

character_logic::character_logic() {
	if (std::filesystem::exists("character_state.json")) {
		ai.load_state("character_state.json");
	}

	// window opened
	character_interaction interaction(character_interaction::kind::WINDOW_OPEN);
	begin_think(interaction);
}

void character_logic::handle_interaction(const character_interaction& interaction) {
	if (state_ == logic_state::IDLE) {
		begin_think(interaction);
	}
	else if (state_ == logic_state::TALKING && visuals.is_speaking()) {
		visuals.finish_speaking(); // mouse down while speaking -> quick skip
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
				state_ = logic_state::AWAITING_CHOICE;

				int num_actions = current_state.actions.size();
				std::string message = "Choose an action:\n";
				for (int i = 0; i < num_actions; i++) {
					message += std::to_string(i + 1) + ") " + current_state.actions[i] + "    ";
				}
				visuals.set_saying(message);
			}
			else {
				visuals.set_saying("");
				state_ = logic_state::IDLE;
			}
		}
	}
}

void character_logic::tick(float delta_time) {
	if (state_ == logic_state::THINKING) {
		if (!visuals.is_speaking()) {
			// constantly think
			display_think();
		}
		if (ai.is_response_ready()) {
			visuals.set_chars_per_second(50.0f);
			current_state = ai.get_response();
			if (!current_state.interactions.empty()) {
				state_ = logic_state::TALKING;
				interaction_index_ = 0;
				display_current_interaction();
				interaction_index_++;
			}
			else {
				state_ = logic_state::IDLE;
				visuals.set_saying("");
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

	visuals.tick(delta_time);
}

void character_logic::shutdown() {
	ai.handle_close_interaction();

	ai.save_state("character_state.json");
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
	ai.handle_interaction_async(interaction);
	state_ = logic_state::THINKING;
}

void character_logic::display_think() {
	// slow effect while thinking
	visuals.set_chars_per_second(2.0f);
	visuals.set_saying(".....");
}

void character_logic::display_current_interaction() {
	if (interaction_index_ < current_state.interactions.size()) {
		const auto& inter = current_state.interactions[interaction_index_];
		visuals.set_saying(inter.saying);
		visuals.set_visual(inter.visual);

		if (inter.new_x != -1 || inter.new_y != -1) {
			// auto handles -1 = no change
			visuals.set_position(inter.new_x, inter.new_y);
		}
		if (inter.new_scale != -1) {
			visuals.set_scale(inter.new_scale);
		}
	}
}
