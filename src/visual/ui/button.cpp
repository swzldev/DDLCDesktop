#include <visual/ui/button.h>

void button::click() {
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
}

unsigned int button::next_id = 1;
