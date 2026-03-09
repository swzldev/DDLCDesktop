#pragma once

#include <chrono>

#include <core/noticon.h>
#include <core/window.h>
#include <behaviour/character_logic.h>
#include <discord/discord_rpc.h>

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
	inline discord_rpc& get_discord() {
		return discord_rpc_;
	}

private:
	widget();

	noticon noticon_;
	window* window_;
	renderer* renderer_;
	discord_rpc discord_rpc_;
	bool running_ = true;

	std::chrono::high_resolution_clock::time_point last_time_;

	character_logic* logic_;

	void update();

	void create_noticon();
	void noticon__double_click();
	void noticon__quit();

	void render();
};
