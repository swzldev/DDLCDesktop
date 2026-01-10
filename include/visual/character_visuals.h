#pragma once

#include <string>
#include <vector>
#include <functional>
#include <vector>

#include <core/renderer.h>
#include <visual/sprite.h>
#include <ddlc/characters.h>

class character_visuals {
public:
	static constexpr auto ASSETS_DIR = "./assets";

public:
	character_visuals(renderer* renderer, ddlc_character character = ddlc_character::MONIKA);

	void tick(float delta_time);

	void draw();

	void set_character(ddlc_character character);

	void set_saying(const std::string& saying);
	void set_saying_immediate(const std::string& saying);
	void set_pose(const std::string& left, const std::string& right);
	void set_expression(const std::string& expression);

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

	inline int add_text_button(const std::string& text, bool single_click, const std::function<void(int)>& on_click) {
		int id = static_cast<int>(text_buttons_.size());
		text_buttons_.push_back({ text, on_click, id, single_click });
		return id;
	}
	inline void remove_text_button(int button_index) {
		if (button_index >= 0 && button_index < static_cast<int>(text_buttons_.size())) {
			text_buttons_.erase(text_buttons_.begin() + button_index);
		}
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
	std::string saying_;
	std::string saying_target_;
	size_t saying_index_ = 0;
	bool is_speaking_ = false;
	float saying_timer_ = 0.0f;
	float chars_per_second_ = 50.0f;

	// buttons
	struct text_button {
		std::string text;
		std::function<void(int)> on_click;
		int id = -1;
		bool single_click = false;
	};
	const text_button* current_button_ = nullptr;
	std::vector<text_button> text_buttons_;

	void draw_all_buttons();

	int on_mouse_click();

	void update_sprites();
};