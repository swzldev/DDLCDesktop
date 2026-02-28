#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include <core/textbox.h>
#include <core/character.h>
#include <settings/settings.h>
#include <logic/character_logic.h>

class widget {
public:
	~widget() = default;

	static widget& get_instance() {
		static widget instance;
		return instance;
	}

	void main_loop();
	void stop();

	inline character_logic* get_logic() {
		return logic_.get();
	}

	std::unique_ptr<textbox> tbox;
	std::vector<std::unique_ptr<character>> characters;
	std::unique_ptr<settings> settings_menu;

private:
	widget();

	std::unique_ptr<character_logic> logic_;

	std::chrono::high_resolution_clock::time_point last_time_;
	bool running_ = true;

	void render();
};
