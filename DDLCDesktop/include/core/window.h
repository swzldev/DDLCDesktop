#pragma once

#include <functional>
#include <vector>
#include <Windows.h>

#include <core/renderer.h>
#include <utility/screen_units.h>

class widget;

struct window_event_data {
	pixels_t cursor_x;
	pixels_t cursor_y;
};

class window_event {
public:
	window_event() = default;

	inline void bind(std::function<void(window_event*, window_event_data)> handler) {
		handlers_.push_back(handler);
	}

	inline void stop_propagation() {
		propagation_stopped_ = true;
	}

	void invoke(window_event_data data);

private:
	std::vector<std::function<void(window_event*, window_event_data)>> handlers_;
	bool propagation_stopped_ = false;
};

class window {
public:
	static constexpr int DEF_WINDOW_HEIGHT = 600;
	static constexpr int DEF_WINDOW_WIDTH = 600;

public:
	window(widget* widget);
	window(widget* widget, int width, int height);
	window(widget* widget, int width, int height, int x, int y);
	~window();

	void show(bool focus = false) const;
	void hide() const;

	void reset();
	void move(pixels_t x, pixels_t y);
	void resize(pixels_t w, pixels_t h);

	inline pixels_t pos_x() const { return pos_x_; }
	inline pixels_t pos_y() const { return pos_y_; }

	inline pixels_t width() const { return width_; }
	inline pixels_t height() const { return height_; }

	inline pixels_t mouse_x() const { return mouse_x_; }
	inline pixels_t mouse_y() const { return mouse_y_; }

	inline float mouse_x_normalized() const { return mouse_x_ / static_cast<float>(width_); }
	inline float mouse_y_normalized() const { return mouse_y_ / static_cast<float>(height_); }

	void poll_events() const;

	inline void close() { should_close_ = true; }
	inline bool should_close() const { return should_close_; }

	// events
	window_event on_mouse_click;
	window_event on_mouse_move;
	window_event on_mouse_down;
	window_event on_mouse_up;

	inline HWND get_hwnd() const { return hwnd_; }
	inline renderer* get_renderer() const { return renderer_; }

	bool draggable = false;

private:
	HWND hwnd_;
	widget* widget_;
	renderer* renderer_;
	bool should_close_ = false;

	pixels_t width_ = DEF_WINDOW_WIDTH;
	pixels_t height_ = DEF_WINDOW_HEIGHT;

	pixels_t pos_x_ = 100;
	pixels_t pos_y_ = 100;

	pixels_t mouse_x_ = 0;
	pixels_t mouse_y_ = 0;

	void update_surface() const;

	void create_window(bool show);
	void create_renderer();

	static POINT down_;
	static bool tracking_;
	static bool dragging_;
	static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
