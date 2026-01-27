#include <visual/ui/button.h>

void button::click() {
	if (is_disabled()) {
		return;
	}

	if (type_ == button_type::CLICK) {
		if (on_click_) {
			on_click_();
		}
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
	else if (type_ == button_type::SWAP) {
		if (!swapped_) {
			if (on_click_) {
				on_click_();
			}
		}
		else {
			if (on_click_alt_) {
				on_click_alt_();
			}
		}
		swapped_ = !swapped_;
	}
}

unsigned int button::next_id = 1;
