#include <core/character.h>

#include <core/widget.h>
#include <core/window.h>
#include <visual/character_visuals.h>

character::character(widget* owner, ddlc_character ddlc_chr) {
	window = std::make_unique<class window>(owner);
	window->draggable = true;
	visuals = std::make_unique<character_visuals>(window.get(), ddlc_chr);
}
