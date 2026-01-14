#pragma once

#include <memory>

#include <visual/character_visuals.h>
#include <behaviour/character_interaction.h>
#include <behaviour/ai/character_ai.h>
#include <ddlc/characters.h>
#include <error/ddlcd_runtime_error.h>

#include <nlohmann/json.hpp>

enum class logic_state {
	IDLE,
	THINKING,
	TALKING,
	AWAITING_CHOICE,
	AWAITING_INPUT,
};

enum class error_state {
	NONE,
	NON_FATAL,
	FATAL,
};

enum class menu_state {
	MAIN,
	SETTINGS,
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
	ddlc_character character_ = ddlc_character::MONIKA;
	bool first_tick_ = true;

	error_state error_state_ = error_state::NONE;
	logic_state state_ = logic_state::IDLE;
	unsigned int interaction_index_ = 0;

	bool input_mode_btn_disabled_ = true;
	bool custom_mode_ = false;
	std::string current_input_;

	// menus
	menu_state current_menu_ = menu_state::MAIN;
	void show_main_menu();
	void show_settings_menu();

	void await_choice(bool show_immediate = false);
	void await_input();

	int get_choice_input(int num_choices);

	void reset_all();

	void begin_think(const character_interaction& interaction);

	void display_think();
	void display_current_interaction();
};