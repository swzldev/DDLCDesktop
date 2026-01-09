#include <core/window.h>

#include <Windows.h>

#include <stdexcept>

window::window() {
	HINSTANCE hInstance = GetModuleHandle(NULL);
	
	// set DPI awareness
	SetProcessDpiAwarenessContext(
		DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
	);

	// window class
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.lpfnWndProc = wnd_proc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"JustMonikaWindowClass";
	wc.hCursor = LoadCursor(NULL, IDC_HAND);
	RegisterClassExW(&wc);

	pos_x_ = 1400;
	pos_y_ = 630;
	width_ = DEF_WINDOW_WIDTH;
	height_ = DEF_WINDOW_HEIGHT;

	// create window
	hwnd_ = CreateWindowExW(
		WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
		L"JustMonikaWindowClass",
		L"",
		WS_POPUP,
		pos_x_, pos_y_,
		width_, height_,
		NULL, NULL, hInstance, this
	);

	// create renderer
	create_renderer();

	// create a timer for update logic
	SetTimer(hwnd_, 1, 16, NULL); // ~60 FPS

	// show window
	show();
}
window::~window() {
	if (hwnd_) {
		DestroyWindow(hwnd_);
		hwnd_ = NULL;
	}
	if (renderer_) {
		delete renderer_;
		renderer_ = nullptr;
	}
}

void window::show(bool focus) const {
	ShowWindow(hwnd_, focus ? SW_SHOW : SW_SHOWNOACTIVATE);
}
void window::hide() const {
	ShowWindow(hwnd_, SW_HIDE);
}

void window::set_position(int x, int y) {
	pos_x_ = x;
	pos_y_ = y;
	SetWindowPos(
		hwnd_,
		HWND_TOPMOST,
		x, y,
		0, 0,
		SWP_NOSIZE | SWP_NOACTIVATE
	);
}
void window::resize(int size) {
	if (size <= 0) {
		throw std::invalid_argument("Size must be positive");
	}
	width_ = size;
	height_ = size;
	SetWindowPos(
		hwnd_,
		HWND_TOPMOST,
		0, 0,
		size, size,
		SWP_NOMOVE | SWP_NOACTIVATE
	);
}

void window::poll_events() const {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void window::create_renderer() {
	if (!renderer_) {
		renderer_ = new renderer(this, hwnd_, width_, height_);
	}
}

void window::invoke(const std::vector<std::function<void()>>& event) const {
	for (const auto& func : event) {
		func();
	}
}

LRESULT window::wnd_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_NCCREATE) {
		auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
		SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(cs->lpCreateParams)
		);
		return TRUE;
	}

	window* win = reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch (uMsg) {
	case WM_NCHITTEST:
	{
		return HTCLIENT;
	}
	case WM_LBUTTONDOWN:
	{
		if (win) {
			win->invoke(win->on_mouse_click);
		}
		return 0;
	}
	case WM_SIZE: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		if (win->renderer_) {
			win->renderer_->resize(x, y);
		}
		return 0;
	}
	case WM_DESTROY:
	{
		if (win) {
			win->should_close_ = true;
		}
		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
