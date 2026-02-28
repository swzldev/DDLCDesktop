#include <gui/controls/gui_checkbox.h>

#include <core/window.h>
#include <core/renderer.h>

gui_checkbox::gui_checkbox(const std::wstring& text, float x, float y, float w, float h, float sz, float outline)
	: text_(text), gui_control(x, y, w, h), size_(sz), outline_(outline) {
	if (!check_sprite_) {
		check_sprite_ = sprite::load_from_file("assets/gui/button/check_selected_foreground.png");
	}
	color_ = D2D1::ColorF(0.6f, 0.6f, 0.6f);
}

void gui_checkbox::draw(renderer* renderer) {
	float check_size = height * 1.4f;

	float aspect_ratio = static_cast<float>(renderer->get_window()->width()) / renderer->get_window()->height();
	float check_width = check_size / aspect_ratio;

	if (checked_) {
		renderer->draw_sprite(check_sprite_, x, y - ((check_size - height) / 2), check_width, check_size);
		renderer->set_text_color(D2D1::ColorF(0.733f, 0.333f, 0.6f));
	}
	else renderer->set_text_color(color_);

	renderer->set_text_alignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	renderer->draw_text(text_, x + width / 2 + check_size, y + height / 2, width - check_size, height, size_, 0.0f, font_family::HALOGEN, 0.04f);
}

void gui_checkbox::on_mouse_enter() {
	color_ = D2D1::ColorF(0.8f, 0.4f, 0.6f);
}
void gui_checkbox::on_mouse_leave() {
	color_ = D2D1::ColorF(0.6f, 0.6f, 0.6f);
}
void gui_checkbox::on_mouse_up() {
	if (checked_ && is_radio_) return;

	set_checked(!checked_);
	if (on_click_) {
		on_click_();
	}
}

void gui_checkbox::update_radio_group() {
	if (radio_group_.empty()) return;

	for (auto checkbox : radio_group_) {
		if (checkbox != this) {
			checkbox->set_checked(false);
		}
	}
}

sprite* gui_checkbox::check_sprite_ = nullptr;
