#pragma once

#include <memory>

#include <core/window.h>
#include <visual/textbox_visuals.h>
#include <settings/settings_controller.h>

class widget;

class settings {
public:
	settings(widget* owner);

	std::unique_ptr<window> window;

	void show();
	void hide();

	void tick(float delta_time);
	void draw() const;

private:
	widget* owner_;
	std::unique_ptr<settings_controller> controller_;
	bool visible_ = false;
};
