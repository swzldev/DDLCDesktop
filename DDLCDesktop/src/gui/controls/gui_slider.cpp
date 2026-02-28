#include <gui/controls/gui_slider.h>

#include <core/window.h>
#include <utility/screen_units.h>

gui_slider::gui_slider(float x, float y, float w, float h, float min, float max)
	: gui_control(x, y, w, h), min_(min), max_(max) {
	if (!slider_bg_) {
		slider_bg_ = sprite::load_from_file("assets/gui/scrollbar/horizontal_poem_bar.png");
	}
	if (!slider_thumb_) {
		slider_thumb_ = sprite::load_from_file("assets/gui/slider/horizontal_hover_thumb.png");
	}
}

void gui_slider::draw(renderer* renderer) {
	float thumb_size = height * 0.25f;
	float aspect_ratio = static_cast<float>(renderer->get_window()->width()) / renderer->get_window()->height();
	float thumb_width = thumb_size / aspect_ratio;

	float slider_aspect = 0.025f;

	renderer->draw_sprite(slider_bg_, x, y, width, width * slider_aspect);
	float thumb_x = x + (((value_ - min_) / (max_ - min_)) * 0.95f) * width;
	renderer->draw_sprite(slider_thumb_, thumb_x, y - 0.003f, thumb_width, thumb_size);
}

bounds gui_slider::get_thumb_bounds(float aspect, float slider_y) {
	float thumb_size = height * 0.25f;
	float thumb_width = thumb_size / aspect;
	float thumb_x = x + (((value_ - min_) / (max_ - min_)) * 0.95f) * width;
	return bounds {
		slider_y - 0.003f,
		slider_y - 0.003f + thumb_size,
		thumb_x,
		thumb_x + thumb_width
	};
}

void gui_slider::on_mouse_move(float mx, float my) {
	if (is_dragging_) {
		float normalized = (mx - x) / width;
		normalized = std::clamp(normalized, 0.0f, 1.0f);

		value_ = min_ + normalized * (max_ - min_);
	}
}
void gui_slider::on_mouse_down() {
	is_dragging_ = true;
}
void gui_slider::on_mouse_up() {
	is_dragging_ = false;
}

sprite* gui_slider::slider_bg_ = nullptr;
sprite* gui_slider::slider_thumb_ = nullptr;
