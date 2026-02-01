#include <logic/states/setup_state.h>

void setup_state::enter()
{
}
void setup_state::exit()
{
}
void setup_state::tick(float delta_time)
{
}
void setup_state::handle_interaction(const character_interaction& interaction)
{
}

std::unique_ptr<iconversation_state> setup_state::next_state()
{
	return std::move(next_state_);
}

void setup_state::advance_step() {
}
void setup_state::display_current_step() {
}
