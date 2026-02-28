#pragma once

#include <vector>
#include <memory>

#include <gui/gui_control.h>

struct gui_view {
	std::vector<std::unique_ptr<gui_control>> controls;
};
