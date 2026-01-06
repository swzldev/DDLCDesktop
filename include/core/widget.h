#pragma once

#include <chrono>

#include <core/window.h>
#include <behaviour/character_logic.h>

class widget {
public:
	~widget();

	static widget& get_instance() {
		static widget instance;
		return instance;
	}

	void main_loop();
	void stop();

private:
	widget();

	window* window_;
	renderer* renderer_;
	bool running_ = true;

	std::chrono::high_resolution_clock::time_point last_time_;

	character_logic character_logic_;
};
