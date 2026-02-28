#pragma once

#include <string>
#include <functional>
#include <d2d1.h>

#include <core/renderer.h>
#include <gui/gui_control.h>

class gui_label : public gui_control {
public:
	gui_label(const std::wstring& text, float x, float y, float sz = 12.0f, float outline = 0.0f, font_family font = font_family::ALLER)
		: text_(text), gui_control(x, y, -1, -1), size_(sz), outline_(outline), font_(font) {
	}

	inline void set_color(D2D_COLOR_F color) {
		color_ = color;
		draw_color_ = color;
	}
	inline void set_color(D2D_COLOR_F color, D2D_COLOR_F outline_color) {
		color_ = color;
		outline_color_ = outline_color;
		hover_color_ = color;
		hover_outline_color_ = outline_color;
		draw_color_ = color;
		draw_outline_color_ = outline_color;
	}
	inline void set_hover_color(D2D_COLOR_F color, D2D_COLOR_F outline_color) {
		hover_color_ = color;
		hover_outline_color_ = outline_color;
	}

	inline void set_on_click(const std::function<void()>& on_click) {
		on_click_ = on_click;
	}

	inline void set_text(const std::wstring& text) {
		text_ = text;
	}

	inline void set_font(font_family font) {
		font_ = font;
	}

	void draw(renderer* renderer) override;

	bounds get_click_bounds() override {
		return calculated_bounds_;
	}

	void on_mouse_enter() override {
		draw_color_ = hover_color_;
		draw_outline_color_ = hover_outline_color_;
	}
	void on_mouse_leave() override {
		draw_color_ = color_;
		draw_outline_color_ = outline_color_;
	}
	void on_mouse_up() override {
		if (on_click_) {
			on_click_();
		}
	}

private:
	std::wstring text_;
	float size_ = 12.0f;
	float outline_ = 0.0f;
	D2D_COLOR_F color_ = D2D1::ColorF(D2D1::ColorF::White);
	D2D_COLOR_F outline_color_ = D2D1::ColorF(D2D1::ColorF::Black);
	D2D_COLOR_F hover_color_ = D2D1::ColorF(D2D1::ColorF::White);
	D2D_COLOR_F hover_outline_color_ = D2D1::ColorF(D2D1::ColorF::White);
	D2D_COLOR_F draw_color_ = D2D1::ColorF(D2D1::ColorF::White);
	D2D_COLOR_F draw_outline_color_ = D2D1::ColorF(D2D1::ColorF::Black);
	std::function<void()> on_click_ = nullptr;
	bounds calculated_bounds_ = { 0, 0, 0, 0 };
	font_family font_ = font_family::ALLER;
};
