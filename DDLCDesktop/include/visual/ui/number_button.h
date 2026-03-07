#pragma once

#include <vector>
#include <string>
#include <functional>

#include <visual/ui/button.h>

class number_button : public button {
public:
	number_button(const std::string& label, const std::function<void(int)>& on_change, int min = 1, int max = 5)
		: label_(label), on_change_(on_change), min_(min), max_(max) {
		value_ = min;
	}
	number_button(const std::string& label, const std::function<void(int)>& on_change, int min, int max, int start_value)
		: label_(label), on_change_(on_change), min_(min), max_(max) {
		value_ = std::clamp(start_value, min, max);
	}
	number_button(const std::string& label, const std::function<void(int)>& on_change, int min, int max, int* value_ptr)
		: label_(label), on_change_(on_change), min_(min), max_(max) {
		value_ptr_ = value_ptr;
		*value_ptr_ = std::clamp(*value_ptr_, min, max);
	}

	virtual std::string text() const override {
		return label_ + value_str();
	}

	inline void set_value_labels(const std::vector<std::string>& labels) {
		value_labels_ = labels;
	}

	inline void set_value_ptr(int* val_ptr) {
		value_ptr_ = val_ptr;
	}
	inline void set_value(int value) {
		if (value_ptr_) {
			*value_ptr_ = value;
		}
		else value_ = value;
	}
	inline void increment() {
		int cur = get_value();
		int next = cur + 1 > max_ ? min_ : cur + 1;

		if (value_ptr_) {
			(*value_ptr_) = next;
		}
		else value_ = next;
	}
	inline int get_value() const { return value_ptr_ ? *value_ptr_ : value_; }

private:
	std::string label_;
	std::function<void(int)> on_change_;
	int value_ = 0;
	int* value_ptr_ = nullptr;
	int min_ = 0;
	int max_ = 5;
	std::vector<std::string> value_labels_;

	virtual void on_click() override {
		increment();
		if (on_change_) {
			on_change_(get_value());
		}
	}

	std::string value_str() const {
		int v = get_value();
		if (!value_labels_.empty() && v - min_ < value_labels_.size()) {
			return value_labels_[v - min_];
		}
		return std::to_string(v);
	}
};
