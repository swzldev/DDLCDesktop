#pragma once

#include <string>
#include <vector>
#include <functional>

#include <core/renderer.h>
#include <visual/sprite.h>
#include <visual/ui/button.h>
#include <ddlc/characters.h>

class character_visuals {
public:
	static constexpr auto ASSETS_DIR = "./assets";

public:
	character_visuals(renderer* renderer, ddlc_character character = ddlc_character::MONIKA);

	void tick(float delta_time);
	void draw();

	void reset(ddlc_character character);

	void set_character(ddlc_character character);

	void set_saying(const std::string& saying);
	void set_saying_immediate(const std::string& saying);
	void set_style(const std::string& style);
	void set_pose(const std::string& left, const std::string& right);
	void set_expression(const std::string& expression);

	inline bool in_popup() const {
		return popup_;
	}
	void show_popup(const std::string& message, const std::function<void(int)>& callback, const std::vector<std::string>& options = { "Yes", "No" });
	void show_message(const std::string& message, const std::function<void()>& callback = nullptr);

	void set_position(int x, int y);
	int get_x();
	int get_y();
	void set_scale(int scale);
	int get_scale();

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

	inline void add_button(const button& btn, bool permanent) {
		if (permanent) {
			buttons_permanent_.push_back(btn);
		}
		else buttons_.push_back(btn);
	}
	inline void clear_buttons() {
		buttons_.clear();
	}

private:
	window* window_ = nullptr;
	renderer* renderer_ = nullptr;

	ddlc_character character_;
	std::string pose_left_ = "1";
	std::string pose_right_ = "1";
	std::string expression_ = "a";
	bool casual_clothing_ = false;

	sprite* head_ = nullptr;
	sprite* body_left_ = nullptr;
	sprite* body_right_ = nullptr;

	// text
	sprite* textbox_ = nullptr;
	std::wstring saying_;
	std::wstring saying_target_;
	size_t saying_index_ = 0;
	bool is_speaking_ = false;
	float saying_timer_ = 0.0f;
	float chars_per_second_ = 50.0f;

	// buttons
	std::vector<button> buttons_permanent_;
	std::vector<button> buttons_;
	button* current_button_ = nullptr;
	int current_option_ = -1;

	// popup
	sprite* popup_bg_ = nullptr;
	std::wstring popup_message_;
	std::vector<std::wstring> popup_options_;
	bool popup_ = false;
	int popup_response_ = -1;
	int num_popup_options_ = 2;
	std::function<void(int)> popup_callback_ = nullptr;

	void draw_all_buttons();
	void draw_popup();

	int on_mouse_click();

	void update_sprites();
};