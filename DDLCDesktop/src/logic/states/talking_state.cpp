#include <logic/states/talking_state.h>

#include <memory>
#include <logic/states/idle_state.h>

void talking_state::enter() {
	logic_->reset_chars_per_second();
	interaction_index_ = 0;
	auto_timer_ = 0.0f;
	display_current_interaction();
}
void talking_state::exit() {
}
void talking_state::tick(float delta_time) {
	if (auto_mode_ && !logic_->visuals->is_speaking()) {
		auto_timer_ += delta_time;
		if (auto_timer_ >= 3.0f) {
			advance_interaction();
		}
	}
}
void talking_state::handle_interaction(const character_interaction& interaction) {
	if (interaction.get_kind() == character_interaction::kind::CLICK) {
		if (logic_->visuals->is_speaking()) {
			// skip animation
			logic_->visuals->finish_speaking();
		}
		else {
			advance_interaction();
		}
	}
}

std::unique_ptr<iconversation_state> talking_state::next_state() {
	return std::move(next_state_);
}

void talking_state::advance_interaction() {
	if (++interaction_index_ < state_.interactions.size()) {
		display_current_interaction();
	}
	else {
		if (!state_.actions.empty()) {
			next_state_ = std::make_unique<idle_state>(logic_);
		}
		else {
			// end interaction
			next_state_ = std::make_unique<idle_state>(logic_);
		}
	}
}
void talking_state::display_current_interaction() {
	const auto& interaction = state_.interactions[interaction_index_];

	logic_->visuals->set_expression(interaction.expression);
	logic_->visuals->set_pose(interaction.pose_left, interaction.pose_right);
	logic_->visuals->set_saying(interaction.saying);

	// TODO: position/scale changes
}
