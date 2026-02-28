#include <visual/textbox_visuals.h>

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

textbox_visuals::textbox_visuals(window* window) {
	window_ = window;
	renderer_ = window->get_renderer();

	window_->on_mouse_click.bind([this](window_event* event, window_event_data) {
		if (on_mouse_click() != 0) {
			event->stop_propagation();
		}
	});

	// load textbox sprite
	std::string textbox_path = (fs::path(ASSETS_DIR) / "gui/textbox.png").make_preferred().string();
	textbox_ = sprite::load_from_file(textbox_path);
	if (!textbox_) {
		throw std::runtime_error("Failed to load textbox sprite from " + textbox_path);
	}
}

void textbox_visuals::tick(float delta_time) {
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

void textbox_visuals::draw() {
	if (!renderer_ || !is_visible_) {
		return;
	}

	// draw textbox background
	renderer_->draw_sprite(textbox_, 0, 0, 1.0f, 1.0f);

	// draw text buttons
	draw_all_buttons();

	// draw text
	if (!saying_target_.empty()) {
		renderer_->set_text_alignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		renderer_->set_text_color(D2D1::ColorF(D2D1::ColorF::White));
		renderer_->set_stroke_color(D2D1::ColorF(0, 0, 0, 0.3f));
		renderer_->draw_text(saying_, 0.5f, 0.5f, 0.91f, 0.63f, 16.0f, 5.0f);
	}
}

void textbox_visuals::set_saying(const std::string& saying) {
	saying_target_ = utf8_to_wstring(saying);
	saying_ = L""; // reset
	saying_index_ = 0;
	saying_timer_ = 0.0f;
	is_speaking_ = true;
}
void textbox_visuals::set_saying_immediate(const std::string& saying) {
	saying_target_ = utf8_to_wstring(saying);
	saying_ = saying_target_;
	saying_index_ = saying_target_.length();
	saying_timer_ = 0.0f;
	is_speaking_ = false;
}

void textbox_visuals::draw_all_buttons() {
	const float button_pad = 0.01f;
	const float buttons_y = 0.85f;

	struct button_draw_data {
		int btn_id;
		std::wstring text;
		float width;
		float height;
		bool is_disabled;
		bool is_toggled;
		// ^^ including padding
	};
	std::vector<button_draw_data> predraw_data;

	float total_width = 0.0f; // normalized & with padding
	float height = 0.0f;

	for (auto& button : buttons_) {
		std::wstring wtext = utf8_to_wstring(button.text());

		// measure (size 2.2)
		D2D1_SIZE_F text_size = renderer_->measure_text(wtext, 12.0f);

		float width_normalized = text_size.width / window_->width() + button_pad * 2;
		float height_normalized = text_size.height / window_->height();
		// ^^ convert to normalized width (0-1)

		height = std::max(height, height_normalized);
		total_width += width_normalized;

		predraw_data.push_back({ button.id(), wtext, width_normalized, height_normalized, button.is_disabled(), button.is_toggled() });
	}

	D2D_COLOR_F btn_col = D2D1::ColorF(0, 0, 0, 0.7f);

	float bx = 0.5f - (total_width / 2.0f);
	for (const auto& data : predraw_data) {
		// change color if hovered
		bool hovered = false;
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

		if (hovered) {
			current_button_id_ = data.btn_id;
			btn_col = D2D1::ColorF(1, 1, 1, 0.65f);
		}
		else {
			if (current_button_id_ == data.btn_id) {
				current_button_id_ = -1;
			}
			btn_col = D2D1::ColorF(0.333f, 0.137f, 0.137f, 1);
		}

		if (data.is_disabled) {
			btn_col = D2D1::ColorF(0.333f, 0.137f, 0.137f, 0.35f);
		}
		else if (data.is_toggled) {
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
			12.0f
			/* no stroke */
		);

		// advance x
		bx += data.width;
	}
}

int textbox_visuals::on_mouse_click() {
	if (current_button_id_ != -1) {
		// find button by ID
		for (auto& btn : buttons_) {
			if (btn.id() == current_button_id_) {
				btn.click();
				return 1; // handled
			}
		}
	}
	return 0; // not handled
}
