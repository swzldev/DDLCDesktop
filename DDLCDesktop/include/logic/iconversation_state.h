#pragma once

#include <memory>

#include <behaviour/character_interaction.h>

class iconversation_state {
public:
	virtual ~iconversation_state() = default;

	virtual void enter() = 0;
	virtual void exit() = 0;
	virtual void tick(float delta_time) = 0;
	virtual void handle_interaction(const character_interaction& interaction) = 0;

	virtual std::unique_ptr<iconversation_state> next_state() = 0;
};
