#include <logic/states/settings_state.h>

#include <core/widget.h>
#include <core/renderer.h>

void settings_state::enter() {
	
}
void settings_state::exit()
{
}
void settings_state::tick(float delta_time)
{
}
void settings_state::handle_interaction(const character_interaction& interaction)
{
}

std::unique_ptr<iconversation_state> settings_state::next_state()
{
	return std::move(next_state_);
}
