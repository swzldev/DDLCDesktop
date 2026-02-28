#pragma once

#include <memory>

#include <core/window.h>
#include <ddlc/characters.h>
#include <visual/character_visuals.h>

class widget;

class character {
public:
	character(widget* owner, ddlc_character ddlc_chr = ddlc_character::MONIKA);

	std::unique_ptr<window> window;
	std::unique_ptr<character_visuals> visuals;

	inline void draw() const {
		window->get_renderer()->begin_draw();
		visuals->draw();
		window->get_renderer()->end_draw();
	}

private:
	widget* owner_;
};
