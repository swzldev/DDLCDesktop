#include <visual/button.h>

void button::click() {
	if (type_ == button_type::CLICK) {
		if (on_click_) {
			on_click_();
		}
	}
	else if (type_ == button_type::CLICK_ONCE) {
		if (on_click_) {
			on_click_();
		}
		// disable further clicks by clearing callbacks
		// but - these should really be removed from the ui
		// by the owner
		on_click_ = nullptr;
		on_click_alt_ = nullptr;
	}
	else if (type_ == button_type::TOGGLE) {
		if (toggled_) {
			if (on_click_alt_) {
				on_click_alt_();
			}
		}
		else {
			if (on_click_) {
				on_click_();
			}
		}
		toggled_ = !toggled_;
	}
}

unsigned int button::next_id = 1;
