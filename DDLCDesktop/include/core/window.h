#pragma once

#include <functional>
#include <vector>
#include <Windows.h>

#include <core/renderer.h>

class widget;

class window {
public:
	static constexpr int DEF_WINDOW_HEIGHT = 600;
	static constexpr int DEF_WINDOW_WIDTH = 600;

public:
	window(widget* widget);
	~window();

	void show(bool focus = false) const;
	void hide() const;

	void reset();
	void set_position(int x, int y);
	void resize(int size); // note: size is both width and height

	inline int pos_x() const { return pos_x_; }
	inline int pos_y() const { return pos_y_; }

	inline int size() const { return width_; }

	inline int mouse_x() const { return mouse_x_; }
	inline int mouse_y() const { return mouse_y_; }

	inline float mouse_x_normalized() const { return mouse_x_ / static_cast<float>(width_); }
	inline float mouse_y_normalized() const { return mouse_y_ / static_cast<float>(height_); }

	void poll_events() const;

	inline void close() { should_close_ = true; }
	inline bool should_close() const { return should_close_; }

	// events
	std::vector<std::function<int()>> on_mouse_click;
	std::vector<std::function<int()>> on_mouse_move;

	inline renderer* get_renderer() const { return renderer_; }

private:
	HWND hwnd_;
	widget* widget_;
	renderer* renderer_;
	bool should_close_ = false;

	int width_ = DEF_WINDOW_WIDTH;
	int height_ = DEF_WINDOW_HEIGHT;

	int pos_x_ = 100;
	int pos_y_ = 100;

	int mouse_x_ = 0;
	int mouse_y_ = 0;

	void update_surface() const;

	void create_renderer();

	void invoke(const std::vector<std::function<int()>>& event) const;

	static POINT down_;
	static bool tracking_;
	static bool dragging_;
	static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
