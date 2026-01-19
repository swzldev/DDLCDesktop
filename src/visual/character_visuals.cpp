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

	// load frame sprite
	std::string frame_path = (fs::path(ASSETS_DIR) / "gui/frame.png").make_preferred().string();
	popup_bg_ = sprite::load_from_file(frame_path);
	if (!popup_bg_) {
		throw std::runtime_error("Failed to load popup background sprite from " + frame_path);
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

	// - draw character -
	renderer_->draw_sprite(body_left_);
	renderer_->draw_sprite(body_right_);
	renderer_->draw_sprite(head_, 0, 0.001f);

	// - draw textbox -
	// draw textbox background
	renderer_->draw_sprite(textbox_, 0, 0.7f);

	// draw text buttons
	draw_all_buttons();

	// draw text
	if (!saying_target_.empty()) {
		renderer_->set_text_alignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		renderer_->set_text_color(D2D1::ColorF(D2D1::ColorF::White));
		renderer_->set_stroke_color(D2D1::ColorF(0, 0, 0, 0.3f));
		renderer_->draw_text(saying_, 0.5f, 0.88f, 0.91f, 0.3f, 2.6f, 5.0f);
	}

	draw_popup();
}

void character_visuals::reset(ddlc_character character) {
	saying_ = L"";
	saying_target_ = L"";
	saying_index_ = 0;
	saying_timer_ = 0.0f;
	is_speaking_ = false;

	buttons_.clear();
	current_button_ = nullptr;

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

void character_visuals::set_saying(const std::string& saying) {
	saying_target_ = utf8_to_wstring(saying);
	saying_ = L""; // reset
	saying_index_ = 0;
	saying_timer_ = 0.0f;
	is_speaking_ = true;
}
void character_visuals::set_saying_immediate(const std::string& saying) {
	saying_target_ = utf8_to_wstring(saying);
	saying_ = saying_target_;
	saying_index_ = saying_target_.length();
	saying_timer_ = 0.0f;
	is_speaking_ = false;
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

void character_visuals::show_popup(const std::string& message, const std::function<void(int)>& callback, const std::vector<std::string>& options) {
	popup_options_.clear();
	popup_message_ = utf8_to_wstring(message);
	for (const std::string& opt : options) {
		popup_options_.push_back(utf8_to_wstring(opt));
	}
	popup_callback_ = callback;
	popup_ = true;
}

void character_visuals::show_message(const std::string& message, const std::function<void()>& callback) {
	show_popup(message, [callback](bool) {
		if (callback) {
			callback();
		}
	}, { "OK" });
}

void character_visuals::set_position(int x, int y) {
	window_->set_position(x, y);
}
int character_visuals::get_x() {
	return window_->pos_x();
}
int character_visuals::get_y() {
	return window_->pos_y();
}
void character_visuals::set_scale(int scale) {
	window_->resize(scale);
}
int character_visuals::get_scale() {
	return window_->size();
}

void character_visuals::draw_all_buttons() {
	const float button_pad = 0.01f;
	const float buttons_y = 0.86f;

	struct button_draw_data {
		button* btn;
		std::wstring text;
		float width;
		float height;
		// ^^ including padding
	};
	std::vector<button_draw_data> predraw_data;

	float total_width = 0.0f; // normalized & with padding
	float height = 0.0f;

	for (auto& button : buttons_) {
		std::wstring wtext = utf8_to_wstring(button.text());

		// measure (size 2.2)
		D2D1_SIZE_F text_size = renderer_->measure_text(wtext, 2.2f);

		float width_normalized = text_size.width / window_->size() + button_pad * 2;
		float height_normalized = text_size.height / window_->size();
		// ^^ convert to normalized width (0-1)

		height = std::max(height, height_normalized);
		total_width += width_normalized;

		predraw_data.push_back({ &button, wtext, width_normalized, height_normalized});
	}

	D2D_COLOR_F btn_col = D2D1::ColorF(0, 0, 0, 0.7f);

	float bx = 0.5f - (total_width / 2.0f);
	for (const auto& data : predraw_data) {
		// change color if hovered
		bool hovered = false;
		if (!popup_) { // ignore inputs in popup
			float mx = window_->mouse_x_normalized();
			float my = window_->mouse_y_normalized();

			float button_center_x = bx + (data.width / 2.0f);

			float left = button_center_x - (data.width / 2.0f);
			float right = button_center_x + (data.width / 2.0f) - button_pad * 2;
			float top = buttons_y - (data.height / 2.0f);
			float bottom = buttons_y + (data.height / 2.0f);

			if (mx >= left && mx <= right && my >= top && my <= bottom) {
				hovered = true;
			}
		}

		if (hovered) {
			current_button_ = data.btn;
			btn_col = D2D1::ColorF(1, 1, 1, 0.65f);
		}
		else {
			if (current_button_ == data.btn) {
				current_button_ = nullptr;
			}
			btn_col = D2D1::ColorF(0.333f, 0.137f, 0.137f, 1);
		}

		if (data.btn && data.btn->is_disabled()) {
			btn_col = D2D1::ColorF(0.333f, 0.137f, 0.137f, 0.35f);
		}
		else if (data.btn && data.btn->is_toggled()) {
			btn_col = D2D1::ColorF(1, 1, 1);
		}

		renderer_->set_text_alignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		renderer_->set_text_color(btn_col);

		// label only button
		renderer_->draw_text(
			data.text,
			bx + (data.width / 2),
			buttons_y,
			data.width,
			data.height,
			2.2f
			/* no stroke */
		);

		// advance x
		bx += data.width;
	}
}
void character_visuals::draw_popup() {
	if (!popup_) return;

	const float popup_width = 0.6f;
	const float popup_height = 0.25f;
	const float popup_x = 0.5f - (popup_width / 2.0f);
	const float popup_y = 0.5f - (popup_height / 2.0f);

	renderer_->draw_sprite(popup_bg_, popup_x, popup_y, popup_width, popup_height);

	// title
	renderer_->set_text_alignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	renderer_->set_text_color(D2D1::ColorF(D2D1::ColorF::Black));
	renderer_->draw_text(popup_message_, 0.5f, 0.51f, popup_width - 0.05f, 0.2f, 3);

	// buttons
	const float button_padding = 0.03f;
	const float button_gap = 0.02f;
	const float buttons_y = popup_y + popup_height * 0.85f;

	float mx = window_->mouse_x_normalized();
	float my = window_->mouse_y_normalized();

	std::vector<float> widths;
	std::vector<float> heights;
	widths.reserve(popup_options_.size());
	heights.reserve(popup_options_.size());

	float total_width = 0.0f;
	for (const auto& opt : popup_options_) {
		auto m = renderer_->measure_text(opt, 3.5f);
		float w = (m.width / window_->size()) + button_padding * 2.0f;
		float h = (m.height / window_->size()) + button_padding * 2.0f;
		widths.push_back(w);
		heights.push_back(h);
		total_width += w;
	}
	if (!popup_options_.empty()) {
		total_width += button_gap * static_cast<float>(popup_options_.size() - 1);
	}

	float bx = 0.5f - (total_width / 2.0f);
	current_option_ = -1;

	auto stroke_col = D2D1::ColorF(0.733f, 0.333f, 0.6f);
	auto stroke_col_hover = D2D1::ColorF(1, 0.666f, 0.8f);
	renderer_->set_text_color(D2D1::ColorF(D2D1::ColorF::White));
	renderer_->set_text_alignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	for (size_t i = 0; i < popup_options_.size(); ++i) {
		float w = widths[i];
		float h = heights[i];
		float center_x = bx + w * 0.5f;

		float left = bx + button_padding / 2;
		float right = bx + w - button_padding;
		float top = buttons_y - h * 0.5f;
		float bottom = buttons_y;

		bool hovered = (mx >= left && mx <= right && my >= top && my <= bottom);
		if (hovered) {
			current_option_ = static_cast<int>(i);
		}

		renderer_->set_stroke_color(hovered ? stroke_col_hover : stroke_col);
		renderer_->draw_text(popup_options_[i], center_x, buttons_y, w, h, 3.5f, 8.0f);

		bx += w + button_gap;
	}
}

int character_visuals::on_mouse_click() {
	if (current_button_) {
		current_button_->click();
		return 1; // handled
	}
	if (popup_ && current_option_ != -1) {
		popup_response_ = (current_option_ == 1);
		popup_ = false;
		if (popup_callback_) {
			popup_callback_(popup_response_);
		}
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

	std::string pose_left_full = pose_left_;;
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
