#include <gui/gui_controller.h>

void gui_controller::show_overlay(sprite* bg_sprite, float w, float h, float x, float y) {
	overlay_sprite_ = bg_sprite;
	overlay_width_ = w;
	overlay_height_ = h;
	overlay_x_ = x;
	overlay_y_ = y;
}
void gui_controller::hide_overlay() {
	overlay_sprite_ = nullptr;
}

void gui_controller::tick(float delta_time) {
	if (overlay_sprite_ && current_view_) {
		for (const auto& control : current_view_->controls) {
			control->tick(delta_time);
		}
	}
}
void gui_controller::render() {
	if (overlay_sprite_) {
		renderer_->draw_sprite(overlay_sprite_, overlay_x_, overlay_y_, overlay_width_, overlay_height_);

		if (current_view_) {
			for (const auto& control : current_view_->controls) {
				control->draw(renderer_);
			}
		}
	}
}

void gui_controller::handle_mouse_move(float x, float y) {
	if (mdown_control_) {
		mdown_control_->on_mouse_move(x, y);
		return;
	}

	if (current_view_) {
		bool hovering_any = false;
		for (const auto& control : current_view_->controls) {
			bounds click_bounds = control->get_click_bounds();
			if (click_bounds.contains(x, y)) {
				hovering_any = true;
				if (hovered_control_ && hovered_control_ != control.get()) {
					hovered_control_->on_mouse_leave();
				}
				control->on_mouse_move(x, y);
				hovered_control_ = control.get();
				control->on_mouse_enter();
				break;
			}
		}
		if (!hovering_any && hovered_control_) {
			hovered_control_->on_mouse_leave();
			hovered_control_ = nullptr;
		}
	}
}
void gui_controller::handle_mouse_down(float x, float y) {
	if (current_view_) {
		for (const auto& control : current_view_->controls) {
			bounds click_bounds = control->get_click_bounds();
			if (click_bounds.contains(x, y)) {
				control->on_mouse_down();
				mdown_control_ = control.get();
				break;
			}
		}
	}
}
void gui_controller::handle_mouse_up(float x, float y) {
	gui_control* clicked_control = nullptr;
	if (mdown_control_) {
		clicked_control = mdown_control_;
		mdown_control_ = nullptr;
	}

	else if (current_view_) {
		for (const auto& control : current_view_->controls) {
			bounds click_bounds = control->get_click_bounds();
			if (click_bounds.contains(x, y)) {
				clicked_control = control.get();
				break;
			}
		}
	}

	// focus handling
	if (clicked_control != focused_control_) {
		if (focused_control_) {
			focused_control_->on_unfocus();
		}

		focused_control_ = clicked_control;
		
		if (focused_control_) {
			focused_control_->on_focus();
		}
	}

	if (clicked_control) {
		clicked_control->on_mouse_up();
	}
}
