#pragma once

#include <memory>

#include <logic/iconversation_state.h>
#include <logic/character_logic.h>

class talking_state : public iconversation_state {
public:
	talking_state(character_state state, character_logic* logic)
		: state_(state), logic_(logic) {
	}

	void enter() override;
	void exit() override;
	void tick(float delta_time) override;
	void handle_interaction(const character_interaction& interaction) override;

	std::unique_ptr<iconversation_state> next_state() override;

private:
	character_logic* logic_;

	std::unique_ptr<iconversation_state> next_state_;

	character_state state_;
	int interaction_index_;
	float auto_timer_;
	bool auto_mode_;

	void advance_interaction();
	void display_current_interaction();
};
