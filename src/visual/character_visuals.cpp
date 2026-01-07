#include <visual/character_visuals.h>

#include <filesystem>
#include <string>
#include <stdexcept>

#include <core/widget.h>

namespace fs = std::filesystem;

void character_visuals::tick(float delta_time) {
	if (saying_index_ < saying_target_.length()) {
		saying_timer_ += delta_time;

		float time_per_char = 1.0f / chars_per_second_;

		// advance characters
		while (saying_timer_ >= time_per_char && saying_index_ < saying_target_.length()) {
			saying_timer_ -= time_per_char;
			saying_index_++;
			saying_ = saying_target_.substr(0, saying_index_);
		}
	}
	else {
		is_speaking_ = false;
	}
}

void character_visuals::draw(renderer* renderer) const {
	if (!renderer) {
		return;
	}

	// draw body parts
	renderer->draw_sprite(body_left_);
	renderer->draw_sprite(body_right_);
	renderer->draw_sprite(head_);

	// draw saying text
	if (!saying_target_.empty()) {
		// draw textbox background
		renderer->draw_sprite(textbox_, 0, 325);

		// draw text
		std::wstring wtext(saying_.begin(), saying_.end());
		renderer->draw_text(wtext, 225, 400, 430, 130, 3);
	}
}

void character_visuals::set_character(ddlc_character character) {
	character_ = character;
	update_sprites();
}

void character_visuals::set_saying(const std::string& saying) {
	saying_target_ = saying;
	saying_index_ = 0;
	saying_timer_ = 0.0f;
	saying_target_ = saying;
	is_speaking_ = true;
}
void character_visuals::set_visual(const std::string visual_str) {
	std::string vis = visual_str;

	// safety checks
	if (vis.size() != 3) {
		vis = "11a";
	}
	if (vis[0] != '1' || vis[0] != '2') {
		vis[0] = '1';
	}
	if (vis[1] != '1' || vis[1] != '2') {
		vis[1] = '1';
	}
	if (vis[2] < 'a' || vis[2] > 'r') {
		vis[2] = 'a';
	}

	pose_left_ = std::string(1, visual_str[0]);
	pose_right_ = std::string(1, visual_str[1]);
	expression_ = visual_str[2];
	update_sprites();
}
void character_visuals::set_pose(const std::string& left, const std::string& right) {
	pose_left_ = left;
	pose_right_ = right;
	update_sprites();
}
void character_visuals::set_expression(const std::string& expression) {
	expression_ = expression;
	update_sprites();
}

void character_visuals::set_position(int x, int y) {
	widget::get_instance().set_position(x, y);
}
int character_visuals::get_x() {
	return widget::get_instance().get_position_x();
}
int character_visuals::get_y() {
	return widget::get_instance().get_position_y();
}
void character_visuals::set_scale(int scale) {
	widget::get_instance().resize(scale);
}
int character_visuals::get_scale() {
	return widget::get_instance().size();
}

void character_visuals::update_sprites() {
	fs::path chr_images_path = fs::path(ASSETS_DIR) / "images";
	switch (character_) {
	case ddlc_character::MONIKA:
		chr_images_path /= "monika";
		break;
	case ddlc_character::YURI:
		chr_images_path /= "yuri";
		break;
	case ddlc_character::NATSUKI:
		chr_images_path /= "natsuki";
		break;
	default:
		throw std::runtime_error("Unsupported character in character_visuals::update_sprites");
	}

	auto normalize_path = [](const fs::path& p) {
		return fs::absolute(fs::weakly_canonical(p).make_preferred());
	};

	fs::path head_path = normalize_path(chr_images_path / (expression_ + ".png"));
	fs::path bl_path = normalize_path(chr_images_path / (pose_left_ + "l.png"));
	fs::path br_path = normalize_path(chr_images_path / (pose_right_ + "r.png"));

	if (!fs::exists(head_path) || !fs::exists(bl_path) || !fs::exists(br_path)) {
		throw std::runtime_error("Sprite file not found in character_visuals::update_sprites");
	}

	head_ = sprite::load_from_file(head_path.string());
	body_left_ = sprite::load_from_file(bl_path.string());
	body_right_ = sprite::load_from_file(br_path.string());

	// load textbox sprite (if needed)
	if (!textbox_) {
		fs::path textbox_path = normalize_path(fs::path(ASSETS_DIR) / "gui/textbox.png");
		if (fs::exists(textbox_path)) {
			textbox_ = sprite::load_from_file(textbox_path.string());
		}
	}
}
