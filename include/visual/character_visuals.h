#pragma once

#include <string>

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
	void set_pose(char left, char right);
	void set_expression(char expression);

	void set_position(int x, int y);
	void set_scale(int scale);

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

	void update_sprites();
};