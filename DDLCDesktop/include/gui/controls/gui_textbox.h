#pragma once

#include <string>
#include <functional>
#include <d2d1.h>

#include <gui/gui_control.h>

class gui_textbox : public gui_control {
public:
	static constexpr float CARET_TOGGLE_INTERVAL = 0.5f;

public:
	gui_textbox(float x, float y, float w, float h, float sz = 12.0f, int limit = 50);
	~gui_textbox();

	inline const std::wstring& get_text() const {
		return input_text_;
	}
	inline void set_text(const std::wstring& text) {
		input_text_ = text;
		if (on_text_changed_) {
			on_text_changed_();
		}
	}
	inline void set_on_text_changed(const std::function<void()>& on_text_changed) {
		on_text_changed_ = on_text_changed;
	}

	void tick(float delta_time) override;
	void draw(renderer* renderer) override;

	bounds get_click_bounds() override {
		return bounds(y, y + height, x, x + width);
	}

	void on_focus() override;
	void on_unfocus() override;

private:
	static sprite* background_sprite_;

	float size_ = 12.0f;
	std::function<void()> on_text_changed_ = nullptr;
	std::wstring input_text_;
	int char_limit_ = 50;
	bool recording_input_ = false;
	float timer_ = 0.0f;
	bool show_caret_ = true;
	bool focused_ = false;

	void input_recorder(wchar_t c);
};
