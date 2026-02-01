#include <logic/states/idle_state.h>

#include <logic/states/thinking_state.h>

void idle_state::enter() {
	logic_->visuals->set_saying_immediate("");
}
void idle_state::exit() {
}
void idle_state::tick(float delta_time) {
	// nothing
}
void idle_state::handle_interaction(const character_interaction& interaction) {
	if (interaction.get_kind() == character_interaction::kind::CLICK) {
		next_state_ = std::make_unique<thinking_state>(logic_);
	}
}

std::unique_ptr<iconversation_state> idle_state::next_state() {
	return std::move(next_state_);
}
