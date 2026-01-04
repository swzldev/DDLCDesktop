#include <core/widget.h>

#include <chrono>

#include <core/window.h>
#include <core/renderer.h>
#include <core/input.h>
#include <behaviour/character_logic.h>
#include <behaviour/character_interaction.h>
#include <visual/sprite.h>

widget::widget() {
	window_ = new window();
	renderer_ = window_->get_renderer();
}
widget::~widget() {
	if (window_) {
		delete window_;
		window_ = nullptr;
	}
}

void widget::main_loop() {
	running_ = true;

	// set hooks
	window_->on_mouse_click.push_back([this]() {
		character_interaction interaction(character_interaction::kind::CLICK);
		character_logic_.handle_interaction(interaction);
	});

	while (running_) {
		auto now = std::chrono::high_resolution_clock::now();
		float delta_time = std::chrono::duration<float>(now - last_time_).count();

		input::tick();

		window_->poll_events();
		if (window_->should_close() || input::is_key_pressed(VK_ESCAPE)) {
			stop();
		}

		renderer_->begin_draw();

		character_logic_.tick(delta_time);
		character_logic_.visuals.draw(renderer_);

		renderer_->end_draw();

		last_time_ = now;
	}

	sprite::cleanup_all_sprites();
}
void widget::stop() {
	character_logic_.shutdown();

	running_ = false;
}
