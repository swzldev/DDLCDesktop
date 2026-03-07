#pragma once

#include <string>
#include <functional>
#include <memory>

#include <visual/sprite.h>

class button {
public:	
	inline button()
		: id_(next_id++) {
	}

	virtual std::string text() const = 0;

	// once set, underlying disabled_ is unused
	inline void set_disabled_ptr(bool* disabled_ptr) {
		disabled_ptr_ = disabled_ptr;
	}
	inline void disable() { disabled_ = true; }
	inline void enable() { disabled_ = false; }
	inline bool is_disabled() const { return disabled_ptr_ ? *disabled_ptr_ : disabled_; }

	inline int id() const {
		return id_;
	}

	virtual void click() {
		if (!is_disabled()) on_click();
	}

private:
	static unsigned int next_id;

private:
	int id_;
	bool disabled_ = false;
	bool* disabled_ptr_ = nullptr;

	virtual void on_click() = 0;
};