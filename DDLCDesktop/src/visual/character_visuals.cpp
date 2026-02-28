#include <visual/character_visuals.h>

#include <filesystem>
#include <string>
#include <stdexcept>

#include <core/window.h>
#include <visual/ui/button.h>

#undef max

namespace fs = std::filesystem;

static std::wstring utf8_to_wstring(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
	return wstr;
}

character_visuals::character_visuals(window* window, ddlc_character character) {
	window_ = window;
	renderer_ = window->get_renderer();
	character_ = character;

	update_sprites();
}

void character_visuals::tick(float delta_time) {
}

void character_visuals::draw() {
	if (!renderer_) {
		return;
	}

	// - draw character -
	renderer_->draw_sprite(body_left_);
	renderer_->draw_sprite(body_right_);
	renderer_->draw_sprite(head_, 0, 0.001f);
}

void character_visuals::reset(ddlc_character character) {
	window_->reset();
	
	character_ = character;
	expression_ = "a";
	pose_left_ = "1";
	pose_right_ = "1";
	update_sprites();
}

void character_visuals::set_character(ddlc_character character) {
	character_ = character;
	update_sprites();
}

void character_visuals::set_style(const std::string& style) {
	if (style == "casual") {
		casual_clothing_ = true;
	}
	else {
		casual_clothing_ = false;
	}
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
	window_->move(x, y);
}
int character_visuals::get_x() {
	return window_->pos_x();
}
int character_visuals::get_y() {
	return window_->pos_y();
}
void character_visuals::set_scale(int scale) {
	window_->resize(scale, scale);
}
int character_visuals::get_scale() {
	return window_->height(); // width and height same for character window
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
	case ddlc_character::SAYORI:
		chr_images_path /= "sayori";
		break;
	default:
		throw std::runtime_error("Failed to get character sprites: unknown character");
	}

	auto normalize_path = [](const fs::path& p) {
		return fs::absolute(fs::weakly_canonical(p).make_preferred());
	};

	std::string pose_left_full = pose_left_;
	std::string pose_right_full = pose_right_;
	if (casual_clothing_) {
		pose_left_full += "b";
		pose_right_full += "b";
	}

	fs::path head_path = normalize_path(chr_images_path / (expression_ + ".png"));
	fs::path bl_path = normalize_path(chr_images_path / (pose_left_full + "l.png"));
	fs::path br_path = normalize_path(chr_images_path / (pose_right_full + "r.png"));

	if (!fs::exists(head_path)) {
		throw std::runtime_error("Head sprite file not found: " + head_path.string());
	}
	else if (!fs::exists(bl_path)) {
		throw std::runtime_error("Pose left sprite file not found: " + bl_path.string());
	}
	else if (!fs::exists(br_path)) {
		throw std::runtime_error("Pose right sprite file not found: " + br_path.string());
	}

	head_ = sprite::load_from_file(head_path.string());
	body_left_ = sprite::load_from_file(bl_path.string());
	body_right_ = sprite::load_from_file(br_path.string());
}
