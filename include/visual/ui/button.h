#pragma once

#include <string>
#include <functional>
#include <memory>

#include <visual/sprite.h>

enum class button_style {
	LABEL,
	IMAGE,
	IMAGE_LABEL,
};

enum class button_type {
	CLICK,
	TOGGLE,
};

class button {
public:	
	inline button(
		const std::string label,
		const std::function<void()>& on_click,
		button_style style = button_style::LABEL,
		button_type type = button_type::CLICK,
		const std::string& label_alt = "",
		const std::function<void()>& on_click_alt = nullptr,
		sprite* img = nullptr
	)
		: style_(style), type_(type),
		text_(label), text_alt_(label_alt),
		img_(img),
		on_click_(on_click), on_click_alt_(on_click_alt),
		id_(next_id++) {
	}

	inline button_style style() const {
		return style_;
	}
	inline button_type type() const {
		return type_;
	}

	inline const std::string& text() const {
		return toggled_ ? text_alt_ : text_;
	}

	inline sprite* img() const {
		return img_.get();
	}

	inline void disable() { disabled_ = true; }
	inline void enable() { disabled_ = false; }
	inline bool is_disabled() const { return disabled_; }

	inline int id() const {
		return id_;
	}

	void click();

private:
	static unsigned int next_id;

private:
	button_style style_;
	button_type type_;

	std::string text_;
	std::string text_alt_;

	std::shared_ptr<sprite> img_;

	std::function<void()> on_click_;
	std::function<void()> on_click_alt_;

	int id_;
	bool toggled_ = false;
	bool disabled_ = false;
};