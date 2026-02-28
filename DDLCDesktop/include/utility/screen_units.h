#pragma once

#include <core/sys.h>

typedef int pixels_t;
typedef float screen_units_t;

namespace screen_units {
	// how many units make up the screen height
	constexpr pixels_t UNIT_COUNT_HEIGHT = 20;

	inline float pixels_per_unit() {
		return static_cast<float>(sys::display_height()) / static_cast<float>(UNIT_COUNT_HEIGHT);
	}

	inline pixels_t to_pixels(screen_units_t su) {
		return static_cast<pixels_t>(su * pixels_per_unit());
	}
	inline screen_units_t from_pixels(pixels_t px) {
		return static_cast<screen_units_t>(px) / pixels_per_unit();
	}	
}