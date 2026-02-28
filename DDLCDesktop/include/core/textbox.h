#pragma once

#include <memory>

#include <core/window.h>
#include <visual/textbox_visuals.h>

class widget;

class textbox {
public:
	textbox(widget* owner);

	std::unique_ptr<window> window;
	std::unique_ptr<textbox_visuals> visuals;

	inline void draw() const {
		window->get_renderer()->begin_draw();
		visuals->draw();
		window->get_renderer()->end_draw();
	}

private:
	widget* owner_;
};
