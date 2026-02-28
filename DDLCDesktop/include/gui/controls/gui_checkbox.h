#pragma once

#include <string>
#include <functional>
#include <d2d1.h>

#include <gui/gui_control.h>

class gui_checkbox : public gui_control {
public:
	gui_checkbox(const std::wstring& text, float x, float y, float w, float h, float sz = 12.0f, float outline = 0.0f);

	inline void set_checked(bool checked) {
		if (is_radio_ && checked) {
			update_radio_group();
		}
		checked_ = checked;

		if (on_checkchanged_) {
			on_checkchanged_(checked_);
		}
	}
	inline bool is_checked() const {
		return checked_;
	}

	inline void set_radio_group(const std::vector<gui_checkbox*>& group, bool def_option = false) {
		if (def_option) checked_ = true;
		radio_group_ = group;
		is_radio_ = true;
	}

	inline void set_on_click(const std::function<void()>& on_click) {
		on_click_ = on_click;
	}
	inline void set_on_checkchanged(const std::function<void(bool)>& on_checkchanged) {
		on_checkchanged_ = on_checkchanged;
	}

	void draw(renderer* renderer) override;

	bounds get_click_bounds() override {
		return bounds(y, y + height, x, x + width);
	}

	void on_mouse_enter() override;
	void on_mouse_leave() override;
	void on_mouse_up() override;

private:
	static sprite* check_sprite_;

	std::wstring text_;
	float size_ = 12.0f;
	float outline_ = 0.0f;
	D2D_COLOR_F color_ = D2D1::ColorF(D2D1::ColorF::White);
	D2D_COLOR_F outline_color_ = D2D1::ColorF(D2D1::ColorF::Black);
	std::function<void()> on_click_ = nullptr;
	std::function<void(bool)> on_checkchanged_ = nullptr;
	bool checked_ = false;
	bool is_radio_ = false;
	std::vector<gui_checkbox*> radio_group_;

	void update_radio_group();
};
