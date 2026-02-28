#include <logic/states/thinking_state.h>

#include <memory>
#include <logic/states/talking_state.h>

void thinking_state::enter() {
	logic_->tbox->visuals->show();
	logic_->tbox->visuals->set_chars_per_second(THINKING_DOTS_SPEED);
	logic_->tbox->visuals->set_saying_immediate("");

	logic_->ai->handle_interaction_async(interaction_);
}
void thinking_state::exit() {
	logic_->reset_chars_per_second();
}
void thinking_state::tick(float delta_time) {
	// repeat "..." infinitely whilst thinking
	if (!error_occurred_ && !logic_->tbox->visuals->is_speaking()) {
		logic_->tbox->visuals->set_saying("... ");
	}

	// poll for response
	if (logic_->ai->is_response_ready()) {
		auto response = logic_->ai->get_response();

		if (response.err != character_state::error::NONE) {
			logic_->reset_chars_per_second();

			std::string message = "An error occurred (code ";
			message += std::to_string(static_cast<int>(response.err));
			message += "). Click to try again.\nIf this continues, please ask for help in the discord server.";
			logic_->tbox->visuals->set_saying(message);
			error_occurred_ = true;
			return;
		}

		next_state_ = std::make_unique<talking_state>(logic_->ai->get_response(), logic_);
	}
}
void thinking_state::handle_interaction(const character_interaction& interaction) {
	if (error_occurred_ && interaction.get_kind() == character_interaction::kind::CLICK) {
		// allow retry on error
		next_state_ = std::make_unique<thinking_state>(logic_, interaction_);
	}
}

std::unique_ptr<iconversation_state> thinking_state::next_state() {
	return std::move(next_state_);
}
