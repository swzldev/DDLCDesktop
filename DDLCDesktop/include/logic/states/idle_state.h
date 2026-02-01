#pragma once

#include <memory>

#include <logic/iconversation_state.h>
#include <logic/character_logic.h>

class idle_state : public iconversation_state {
public:
	idle_state(character_logic* logic)
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
};
