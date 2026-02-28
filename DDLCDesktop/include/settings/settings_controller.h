#pragma once

#include <memory>
#include <unordered_map>

#include <core/window.h>
#include <core/renderer.h>
#include <gui/gui_controller.h>
#include <config/config.h>

enum class settings_menu {
	MAIN
};

class settings;

class settings_controller {
public:
	settings_controller(settings* stngs);

	inline void set_menu(settings_menu menu) {
		current_menu_ = menu;
		update_view();
	}

	void close() const;

	std::unique_ptr<gui_controller> gui;

private:
	config* cfg_;
	settings* stngs_;
	settings_menu current_menu_ = settings_menu::MAIN;
	std::unordered_map<settings_menu, std::unique_ptr<gui_view>> menu_views_;
	sprite* overlay_sprite_ = nullptr;

	void update_view();

	void load_all_menu_views();
	void load_main_menu_view();
};