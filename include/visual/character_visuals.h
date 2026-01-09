#pragma once

#include <string>
#include <vector>
#include <functional>

#include <core/renderer.h>
#include <visual/sprite.h>
#include <ddlc/characters.h>

class character_visuals {
public:
	static constexpr auto ASSETS_DIR = "./assets";

public:
	character_visuals(ddlc_character character = ddlc_character::MONIKA)
		: character_(character) {
		update_sprites();
	}

	void tick(float delta_time);

	void draw(renderer* renderer) const;

	void set_character(ddlc_character character);

	void set_saying(const std::string& saying);
	void set_visual(const std::string visual_str);
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

	void add_text_button(const std::string& text, const std::function<void()>& on_click) {
		text_buttons_.push_back({ text, on_click });
	}

private:
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
		std::function<void()> on_click;
	};
	std::vector<text_button> text_buttons_;

	void draw_all_buttons(renderer* renderer) const;

	void update_sprites();
};