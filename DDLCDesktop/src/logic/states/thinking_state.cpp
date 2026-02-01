#include <logic/states/thinking_state.h>

#include <memory>
#include <logic/states/talking_state.h>

void thinking_state::enter() {
	logic_->visuals->set_chars_per_second(THINKING_DOTS_SPEED);
	logic_->visuals->set_saying("... ");
}
void thinking_state::exit() {
	logic_->reset_chars_per_second();
}
void thinking_state::tick(float delta_time) {
	// repeat "..." infinitely whilst thinking
	if (!logic_->visuals->is_speaking()) {
		logic_->visuals->set_saying("... ");
	}

	// poll for response
	if (logic_->ai->is_response_ready()) {
		next_state_ = std::make_unique<talking_state>(logic_->ai->get_response(), logic_);
	}
}
void thinking_state::handle_interaction(const character_interaction& interaction) {
	// ignore whilst thinking
}

std::unique_ptr<iconversation_state> thinking_state::next_state() {
	return std::move(next_state_);
}
