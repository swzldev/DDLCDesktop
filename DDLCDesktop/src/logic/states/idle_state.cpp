#include <logic/states/idle_state.h>

#include <logic/states/thinking_state.h>

void idle_state::enter() {
	logic_->tbox->visuals->show();
	logic_->tbox->visuals->set_saying_immediate("[IDLE]");

	logic_->tbox->visuals->add_button(button("Close", [this]() {
		logic_->shutdown();
	}));

	logic_->tbox->visuals->add_button(button("Settings", [this]() {
		logic_->settingz->show();
	}));
}
void idle_state::exit() {
}
void idle_state::tick(float delta_time) {
	// nothing
}
void idle_state::handle_interaction(const character_interaction& interaction) {
	if (interaction.get_kind() == character_interaction::kind::CLICK) {
		next_state_ = std::make_unique<thinking_state>(logic_, interaction);
	}
}

std::unique_ptr<iconversation_state> idle_state::next_state() {
	return std::move(next_state_);
}
