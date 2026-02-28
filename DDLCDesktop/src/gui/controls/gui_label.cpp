#include <gui/controls/gui_label.h>

#include <core/renderer.h>
#include <core/window.h>

void gui_label::draw(renderer* renderer) {
	renderer->set_text_color(draw_color_);
	renderer->set_stroke_color(draw_outline_color_);
	renderer->set_text_alignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	float spacing = 0.02f;
	if (font_ == font_family::RIFFIC) {
		spacing = 0.08f;
	}

	auto sz = renderer->measure_text(text_, size_, font_, spacing);
	float w = (sz.width + outline_) / renderer->get_window()->width();
	float h = (sz.height + outline_) / renderer->get_window()->height();

	renderer->draw_text(text_, x + w / 2, y + h / 2, w, h, size_, outline_, font_, spacing);
	calculated_bounds_ = bounds(y, y + h, x, x + w);
}
