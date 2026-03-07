#pragma once

#include <string>
#include <functional>

#include <visual/ui/button.h>

class text_button : public button {
public:
	text_button(const std::string& text, const std::function<void()>& on_click)
		: text_(text) {
		on_click_ = on_click;
	}

	virtual std::string text() const override {
		return text_;
	}

private:
	std::string text_;
	std::function<void()> on_click_;

	virtual void on_click() override {
		if (on_click_) {
			on_click_();
		}
	}
};