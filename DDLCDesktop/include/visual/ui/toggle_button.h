#pragma once

#include <string>
#include <functional>

#include <visual/ui/button.h>

class toggle_button : public button {
public:
	toggle_button(const std::string& label, const std::function<void(bool)>& on_click)
		: label_(label), on_click_(on_click) {
	}
	toggle_button(const std::string& label, const std::function<void(bool)>& on_click, bool initial_state)
		: label_(label), on_click_(on_click), toggled_(initial_state) {
	}
	toggle_button(const std::string& label, const std::function<void(bool)>& on_click, bool* toggled_ptr)
		: label_(label), on_click_(on_click), toggled_ptr_(toggled_ptr) {
	}

	virtual std::string text() const override {
		return label_ + toggled_str();
	}

	inline void set_labels(const std::string& toggled_label, const std::string& untoggled_label) {
		toggled_label_ = toggled_label;
		untoggled_label_ = untoggled_label;
	}

	inline void set_toggled_ptr(bool* toggled_ptr) {
		toggled_ptr_ = toggled_ptr;
	}
	inline void toggle() {
		if (toggled_ptr_) {
			*toggled_ptr_ = !(*toggled_ptr_);
		}
		else toggled_ = !toggled_;
	}
	inline bool is_toggled() const { return toggled_ptr_ ? *toggled_ptr_ : toggled_; }

private:
	std::string label_;
	std::string toggled_label_ = "ON";
	std::string untoggled_label_ = "OFF";
	std::function<void(bool)> on_click_;
	bool toggled_ = false;
	bool* toggled_ptr_ = nullptr;

	virtual void on_click() override {
		toggle();
		if (on_click_) {
			on_click_(is_toggled());
		}
	}

	std::string toggled_str() const {
		return is_toggled() ? toggled_label_ : untoggled_label_;
	}
};
