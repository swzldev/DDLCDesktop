#pragma once

#include <memory>

#include <logic/iconversation_state.h>
#include <logic/character_logic.h>

class setup_state : public iconversation_state {
public:
	setup_state(character_logic* logic)
		: logic_(logic) {
	}

	void enter() override;
	void exit() override;
	void tick(float delta_time) override;
	void handle_interaction(const character_interaction& interaction) override;

	std::unique_ptr<iconversation_state> next_state() override;

private:
	character_logic* logic_;

	std::unique_ptr<iconversation_state> next_state_;

	int interaction_index_;
	int setup_step_index_;

	void advance_step();
	void display_current_step();
};
