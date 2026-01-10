#pragma once

#include <visual/character_visuals.h>
#include <behaviour/character_interaction.h>
#include <behaviour/ai/character_ai.h>
#include <error/ddlcd_runtime_error.h>

enum class logic_state {
	IDLE,
	THINKING,
	TALKING,
	AWAITING_CHOICE,
	AWAITING_INPUT,
};

class window;

class character_logic {
public:
	static constexpr int INPUT_MAX_LENGTH = 200;

public:
	character_logic(window* window);
	~character_logic();

	void handle_interaction(const character_interaction& interaction);

	void tick(float delta_time);

	void handle_error(const ddlcd_runtime_error& error);

	character_visuals* visuals;
	character_ai* ai;
	character_state current_state;

private:
	window* window_ = nullptr;

	bool error_state_ = false;
	logic_state state_ = logic_state::IDLE;
	unsigned int interaction_index_ = 0;

	int actions_button_ = -1;
	int custom_button_ = -1;
	void cleanup_temp_buttons();

	bool custom_mode_ = false;
	std::string current_input_;

	void close_button_click();
	void reset_button_click();
	void custom_button_click();
	void actions_button_click();

	void await_choice(bool show_immediate = false);
	void await_input();

	int get_choice_input(int num_choices);

	void reset_all();

	void begin_think(const character_interaction& interaction);

	void display_think();
	void display_current_interaction();
};