#include <visual/character_visuals.h>

#include <filesystem>
#include <string>
#include <stdexcept>

#include <core/widget.h>

#undef max

namespace fs = std::filesystem;

character_visuals::character_visuals(renderer* renderer, ddlc_character character) {
	renderer_ = renderer;
	window_ = renderer_->get_window();
	character_ = character;

	window_->on_mouse_click.push_back([this]() {
		return on_mouse_click();
	});

	// load textbox sprite
	std::string textbox_path = (fs::path(ASSETS_DIR) / "gui/textbox.png").make_preferred().string();
	textbox_ = sprite::load_from_file(textbox_path);
	if (!textbox_) {
		throw std::runtime_error("Failed to load textbox sprite from " + textbox_path);
	}

	update_sprites();
}

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

void character_visuals::draw() {
	if (!renderer_) {
		return;
	}

	// draw body parts
	renderer_->draw_sprite(body_left_);
	renderer_->draw_sprite(body_right_);
	renderer_->draw_sprite(head_);

	// draw textbox
	if (!saying_target_.empty()) {
		// draw textbox background
		renderer_->draw_sprite(textbox_, 0, 0.7f);

		// draw text buttons
		draw_all_buttons();

		// draw text
		std::wstring wtext(saying_.begin(), saying_.end());
		renderer_->set_text_color(D2D1::ColorF(D2D1::ColorF::White));
		renderer_->set_stroke_color(D2D1::ColorF(D2D1::ColorF::Black));
		renderer_->draw_text(wtext, 0.5f, 0.9f, 0.95f, 0.35f, 3, 6.5f);
	}
}

void character_visuals::set_character(ddlc_character character) {
	character_ = character;
	update_sprites();
}

void character_visuals::set_saying(const std::string& saying) {
	saying_target_ = saying;
	saying_ = ""; // reset
	saying_index_ = 0;
	saying_timer_ = 0.0f;
	saying_target_ = saying;
	is_speaking_ = true;
}
void character_visuals::set_saying_immediate(const std::string& saying) {
	saying_target_ = saying;
	saying_ = saying;
	saying_index_ = saying_target_.length();
	saying_timer_ = 0.0f;
	is_speaking_ = false;
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

void character_visuals::draw_all_buttons() {
	const float button_pad = 0.01f;
	const float buttons_y = 0.865f;

	struct button_predraw_data {
		const text_button* btn;
		std::wstring text;
		float width;
		float height;
		// ^^ including padding
	};
	std::vector<button_predraw_data> predraw_data;

	float total_width = 0.0f; // normalized & with padding
	float height = 0.0f;

	for (const auto& [name, button] : text_buttons_) {
		// convert to wstring
		std::wstring wtext(button.text.begin(), button.text.end());

		// measure (size 2.5)
		D2D1_SIZE_F text_size = renderer_->measure_text(wtext, 2.5f);

		float width_normalized = text_size.width / window_->size() + button_pad * 2;
		float height_normalized = text_size.height / window_->size() + button_pad * 2;
		// ^^ convert to normalized width (0-1)

		height = std::max(height, height_normalized);
		total_width += width_normalized;

		predraw_data.push_back({ &button, wtext, width_normalized, height_normalized });
	}

	D2D_COLOR_F btn_col = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.4f);

	float bx = 0.5f - (total_width / 2.0f);
	for (const auto& data : predraw_data) {
		// change color if hovered
		float mx = window_->mouse_x_normalized();
		float my = window_->mouse_y_normalized();

		float left = bx - button_pad;
		float right = bx + data.width - button_pad * 2;
		float top = buttons_y - (data.height / 2);
		float bottom = buttons_y + (data.height / 2) - button_pad * 2;

		if (mx >= left && mx <= right && my >= top && my <= bottom) {
			current_button_ = data.btn;
			btn_col = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			if (current_button_ == data.btn) {
				current_button_ = nullptr;
			}
			btn_col = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.4f);
		}

		renderer_->set_text_color(btn_col);

		// label only button
		renderer_->draw_text(
			data.text,
			bx + (data.width / 2),
			buttons_y,
			data.width,
			data.height,
			2.5f
			/* no stroke */
		);

		// advance x
		bx += data.width;
	}
}

int character_visuals::on_mouse_click() {
	if (current_button_) {
		current_button_->on_click();
		return 1; // handled
	}
	return 0; // not handled
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

	fs::path head_path = normalize_path(chr_images_path / (expression_ + ".png"));
	fs::path bl_path = normalize_path(chr_images_path / (pose_left_ + "l.png"));
	fs::path br_path = normalize_path(chr_images_path / (pose_right_ + "r.png"));

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
