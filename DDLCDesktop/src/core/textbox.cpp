#include <core/textbox.h>

#include <core/widget.h>
#include <core/window.h>
#include <core/sys.h>
#include <visual/textbox_visuals.h>
#include <utility/screen_units.h>

textbox::textbox(widget* owner) {
	owner_ = owner;

	pixels_t tbox_w = screen_units::to_pixels(16.5f);
	pixels_t tbox_h = screen_units::to_pixels(3);

	pixels_t tbox_x = sys::display_width() / 2 - tbox_w / 2;
	pixels_t tbox_y = sys::display_height() - tbox_h - screen_units::to_pixels(0.5f);

	window = std::make_unique<class window>(owner, tbox_w, tbox_h, tbox_x, tbox_y);
	visuals = std::make_unique<textbox_visuals>(window.get());
}
