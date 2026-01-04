#pragma once

#include <visual/character_visuals.h>
#include <behaviour/character_interaction.h>
#include <behaviour/ai/character_ai.h>

enum class logic_state {
	IDLE,
	THINKING,
	TALKING,
	AWAITING_CHOICE,
};

class character_logic {
public:
	character_logic();

	void handle_interaction(const character_interaction& interaction);

	void tick(float delta_time);

	void shutdown();

	character_visuals visuals;
	character_ai ai;
	character_state current_state;

private:
	logic_state state_ = logic_state::IDLE;
	unsigned int interaction_index_ = 0;

	int get_choice_input(int num_choices);

	void begin_think(const character_interaction& interaction);

	void display_think();
	void display_current_interaction();
};