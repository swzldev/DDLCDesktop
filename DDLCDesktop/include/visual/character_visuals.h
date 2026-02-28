#pragma once

#include <string>
#include <vector>
#include <functional>

#include <core/window.h>
#include <core/renderer.h>
#include <visual/sprite.h>
#include <visual/ui/button.h>
#include <ddlc/characters.h>

class character_visuals {
public:
	static constexpr auto ASSETS_DIR = "./assets";

public:
	character_visuals(window* window, ddlc_character character = ddlc_character::MONIKA);

	void tick(float delta_time);
	void draw();

	void reset(ddlc_character character);

	void set_character(ddlc_character character);

	void set_style(const std::string& style);
	void set_pose(const std::string& left, const std::string& right);
	void set_expression(const std::string& expression);

	void set_position(int x, int y);
	int get_x();
	int get_y();
	void set_scale(int scale);
	int get_scale();

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

	void update_sprites();
};