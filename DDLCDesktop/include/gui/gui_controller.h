#pragma once

#include <core/renderer.h>
#include <gui/gui_view.h>
#include <gui/gui_control.h>
#include <visual/sprite.h>

class gui_controller {
public:
	explicit gui_controller(renderer* renderer)
		: renderer_(renderer) {
	}

	void show_overlay(sprite* bg_sprite, float w = 1.0f, float h = 1.0f, float x = 0.5f, float y = 0.5f);
	void hide_overlay();

	inline void set_view(gui_view* view) { current_view_ = view; }

	void tick(float delta_time);
	void render();

	void handle_mouse_move(float x, float y);
	void handle_mouse_down(float x, float y);
	void handle_mouse_up(float x, float y);

private:
	renderer* renderer_;

	sprite* overlay_sprite_ = nullptr;
	gui_view* current_view_ = nullptr;
	gui_control* hovered_control_ = nullptr;
	gui_control* mdown_control_ = nullptr;
	gui_control* focused_control_ = nullptr;
	float overlay_x_ = 0.5f;
	float overlay_y_ = 0.5f;
	float overlay_width_ = 1.0f;
	float overlay_height_ = 1.0f;
};