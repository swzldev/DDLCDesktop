#pragma once

#include <string>
#include <vector>
#include <functional>

#include <core/window.h>
#include <core/renderer.h>
#include <visual/sprite.h>
#include <visual/ui/button.h>
#include <ddlc/characters.h>

class textbox_visuals {
public:
	static constexpr auto ASSETS_DIR = "./assets";

public:
	textbox_visuals(window* window);

	void tick(float delta_time);
	void draw();

	inline void show() { is_visible_ = true; }
	inline void hide() { is_visible_ = false; }

	void set_saying(const std::string& saying);
	void set_saying_immediate(const std::string& saying);

	inline void set_chars_per_second(float cps) {
		chars_per_second_ = cps;
	}

	inline bool is_speaking() const {
		return is_speaking_;
	}
	inline void finish_speaking() {
		saying_index_ = saying_target_.length();
		saying_ = saying_target_;
		saying_timer_ = 0.0f;
		is_speaking_ = false;
	}

	inline void add_button(const button& btn) {
		buttons_.push_back(btn);
	}
	inline void clear_buttons() {
		buttons_.clear();
	}

private:
	window* window_ = nullptr;
	renderer* renderer_ = nullptr;

	// text
	sprite* textbox_ = nullptr;
	std::wstring saying_;
	std::wstring saying_target_;
	size_t saying_index_ = 0;
	bool is_speaking_ = false;
	bool is_visible_ = false;
	float saying_timer_ = 0.0f;
	float chars_per_second_ = 50.0f;

	// buttons
	std::vector<button> buttons_;
	int current_button_id_ = -1;
	int current_option_ = -1;

	void draw_all_buttons();

	int on_mouse_click();
};