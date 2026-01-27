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

	inline void set_position(int x, int y) {
		window_->set_position(x, y);
	}
	inline void resize(int size) {
		window_->resize(size);
	}

	inline int get_position_x() {
		return window_->pos_x();
	}
	inline int get_position_y() {
		return window_->pos_y();
	}

	inline int size() {
		return window_->size();
	}

	inline character_logic* get_logic() {
		return logic_;
	}

private:
	widget();

	window* window_;
	renderer* renderer_;
	bool running_ = true;

	std::chrono::high_resolution_clock::time_point last_time_;

	character_logic* logic_;

	void render();
};
