#include <core/widget.h>

#include <chrono>
#include <filesystem>

#include <core/window.h>
#include <core/renderer.h>
#include <core/input.h>
#include <behaviour/character_logic.h>
#include <behaviour/character_interaction.h>
#include <discord/discord_rpc.h>
#include <visual/sprite.h>
#include <error/ddlcd_runtime_error.h>

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
		try {
			while (running_) {
				update();
			}
		}
		catch (const ddlcd_runtime_error& e) {
			if (!logic_) {
				throw;
			}

			logic_->handle_error(e);
		}
	}

	sprite::cleanup_all_sprites();
}
void widget::stop() {
	running_ = false;
}

widget::widget() {
	// filesystem checks

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

	// allocate
	window_ = new window(this);
	renderer_ = window_->get_renderer();
	logic_ = new character_logic(this);

	// create noticon (after window)
	create_noticon();
}

void widget::update() {
	// calculate delta time
	auto now = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration<float>(now - last_time_).count();

	// tick input system
	input::tick();

	// poll window events and check to close
	window_->poll_events();
	if (window_->should_close()) {
		stop();
	}

	// tick logic system
	logic_->tick(delta_time);
	render();

	// update last time
	last_time_ = now;
}

void widget::create_noticon() {
	// init
	noticon_.initialize(L"DDLC Desktop");

	// add buttons
	noticon_.add_button(L"Quit", [this]() { noticon__quit(); });

	noticon_.set_on_double_click([this]() { noticon__double_click(); });
}

void widget::noticon__double_click() {
	window_->show(true);
}
void widget::noticon__quit() {
	stop();
}

void widget::render() {
	renderer_->begin_draw();
	logic_->visuals->draw();
	renderer_->end_draw();
}
