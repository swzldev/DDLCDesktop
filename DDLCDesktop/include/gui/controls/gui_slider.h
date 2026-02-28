#pragma once

#include <string>
#include <memory>
#include <algorithm>
#include <functional>
#include <d2d1.h>

#include <gui/gui_control.h>

class gui_slider : public gui_control {
public:
	gui_slider(float x, float y, float w, float h, float min = 0.0f, float max = 1.0f);

	inline void set_value(float value) {
		value_ = std::clamp(value, min_, max_);
	}
	inline float get_value() const {
		return value_;
	}

	void draw(renderer* renderer) override;

	bounds get_click_bounds() override {
		return bounds(y, y + height, x, x + width);
	}
	bounds get_thumb_bounds(float aspect, float slider_y);

	void on_mouse_move(float mx, float my) override;
	void on_mouse_down() override;
	void on_mouse_up() override;

private:
	static sprite* slider_bg_;
	static sprite* slider_thumb_;

	float min_ = 0.0f;
	float max_ = 1.0f;
	float value_ = 1.0f;
	bool is_dragging_ = false;
};
