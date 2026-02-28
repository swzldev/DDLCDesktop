#pragma once

#include <core/renderer.h>
#include <utility/bounds.h>

class gui_control {
public:
	gui_control(float x, float y, float width = 0.0f, float height = 0.0f)
		: x(x), y(y), width(width), height(height) {
	}
	virtual ~gui_control() = default;

	virtual void tick(float delta_time) {}
	virtual void draw(renderer* renderer) = 0;

	virtual bounds get_click_bounds() = 0;

	virtual void on_mouse_move(float x, float y) {}
	virtual void on_mouse_enter() {}
	virtual void on_mouse_leave() {}
	virtual void on_mouse_down() {}
	virtual void on_mouse_up() {}

	virtual void on_focus() {}
	virtual void on_unfocus() {}

	float x, y;
	float width, height;
};
