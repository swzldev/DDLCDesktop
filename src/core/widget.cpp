#include <core/widget.h>

#include <chrono>
#include <filesystem>

#include <core/window.h>
#include <core/renderer.h>
#include <core/input.h>
#include <behaviour/character_logic.h>
#include <behaviour/character_interaction.h>
#include <visual/sprite.h>

namespace fs = std::filesystem;

widget::~widget() {
	if (window_) {
		delete window_;
		window_ = nullptr;
	}
	if (logic_) {
		delete logic_;
		logic_ = nullptr;
	}
}

void widget::main_loop() {
	running_ = true;

	// first tick delta time will be 0, but thats better
	// than some crazy huge value
	last_time_ = std::chrono::high_resolution_clock::now();
	while (running_) {
		auto now = std::chrono::high_resolution_clock::now();
		float delta_time = std::chrono::duration<float>(now - last_time_).count();

		input::tick();

		window_->poll_events();
		if (window_->should_close()) {
			stop();
		}

		renderer_->begin_draw();

		logic_->tick(delta_time);
		logic_->visuals->draw();

		renderer_->end_draw();

		last_time_ = now;
	}

	sprite::cleanup_all_sprites();
}
void widget::stop() {
	running_ = false;
}

widget::widget() {
	// pre-init checks

	// assets folder
	if (!fs::exists("./assets/") || !fs::is_directory("./assets/")) {
		throw std::runtime_error("Assets directory './assets/' not found (you may have a corrupted installation)");
	}
	// images folder
	if (!fs::exists("./assets/images/") || !fs::is_directory("./assets/images/")) {
		throw std::runtime_error("Assets images directory './assets/images/' not found (did you read the installation instructions on the github?)");
	}
	// gui folder
	if (!fs::exists("./assets/gui/") || !fs::is_directory("./assets/gui/")) {
		throw std::runtime_error("Assets GUI directory './assets/gui/' not found (did you read the installation instructions on the github?)");
	}

	// config.json
	if (!fs::exists("config.json") || !fs::is_regular_file("config.json")) {
		throw std::runtime_error("Configuration file 'config.json' not found (you may have a corrupted installation)");
	}

	window_ = new window(this);
	renderer_ = window_->get_renderer();
	logic_ = new character_logic(window_);
}
