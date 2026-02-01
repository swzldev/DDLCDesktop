#pragma once

#include <chrono>

#include <core/window.h>
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
		return logic_.get();
	}

private:
	widget();

	std::unique_ptr<window> window_;
	renderer* renderer_;
	std::unique_ptr<character_logic> logic_;

	std::chrono::high_resolution_clock::time_point last_time_;
	bool running_ = true;

	void render();
};
