#pragma once

#include <functional>
#include <vector>
#include <Windows.h>

#include <core/renderer.h>

class window {
public:
	static constexpr int DEF_WINDOW_HEIGHT = 450;
	static constexpr int DEF_WINDOW_WIDTH = 450;

public:
	window();
	~window();

	void show(bool focus = false) const;
	void hide() const;

	void set_position(int x, int y) const;
	void resize(int size); // note: size is both width and height

	void poll_events() const;

	inline bool should_close() const { return should_close_; }

	std::vector<std::function<void()>> on_mouse_click;

	inline renderer* get_renderer() const { return renderer_; }

private:
	HWND hwnd_;
	renderer* renderer_;
	bool should_close_ = false;
	int width_ = DEF_WINDOW_WIDTH;
	int height_ = DEF_WINDOW_HEIGHT;

	void create_renderer();

	void invoke(const std::vector<std::function<void()>>& event) const;

	static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
