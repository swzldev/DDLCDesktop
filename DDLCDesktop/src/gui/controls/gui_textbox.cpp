#include <gui/controls/gui_textbox.h>

#include <core/window.h>
#include <core/renderer.h>
#include <core/input.h>

gui_textbox::gui_textbox(float x, float y, float w, float h, float sz, int limit)
	: gui_control(x, y, w, h), size_(sz), char_limit_(limit) {
	if (!background_sprite_) {
		background_sprite_ = sprite::load_from_file("assets/gui/button/choice_hover_background.png");
	}
}
gui_textbox::~gui_textbox() {
	if (recording_input_) {
		input::end_input_recording();
	}
}

void gui_textbox::tick(float delta_time) {
	if (!focused_) return;
	timer_ += delta_time;

	if (timer_ >= CARET_TOGGLE_INTERVAL) {
		show_caret_ = !show_caret_;
		timer_ = 0.0f;
	}
}

void gui_textbox::draw(renderer* renderer) {
	renderer->draw_sprite(background_sprite_, x, y, width, height);

	renderer->set_text_color(D2D1::ColorF(D2D1::ColorF::Black));
	renderer->set_text_alignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	std::wstring text = input_text_;
	if (focused_ && show_caret_) {
		text += L'|';
	}

	float txt_h = renderer->measure_text(text, size_).height / renderer->get_window()->height();

	renderer->draw_text(text, x + width / 2 + 0.01f, y + height / 2 + (height - txt_h) / 2, width, height, size_);
}

void gui_textbox::on_focus() {
	input::begin_input_recording([this](wchar_t c) {
		input_recorder(c);
	});
	focused_ = true;
	timer_ = 0.0f;
	show_caret_ = true;
}
void gui_textbox::on_unfocus() {
	input::end_input_recording();
	focused_ = false;
}

void gui_textbox::input_recorder(wchar_t c) {
	if (c == L'\b') {
		if (!input_text_.empty()) {
			input_text_.pop_back();
		}
	}
	else if (c >= 32 && input_text_.size() < char_limit_) {
		input_text_ += c;
	}

	if (on_text_changed_) {
		on_text_changed_();
	}
}

sprite* gui_textbox::background_sprite_ = nullptr;
