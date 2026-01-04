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

	// create window
	hwnd_ = CreateWindowExW(
		WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
		L"JustMonikaWindowClass",
		L"",
		WS_POPUP,
		1400, 630,
		WINDOW_WIDTH, WINDOW_HEIGHT,
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

void window::poll_events() const {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void window::create_renderer() {
	if (!renderer_) {
		renderer_ = new renderer(hwnd_, WINDOW_WIDTH, WINDOW_HEIGHT);
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
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(hwnd, &pt);

		if (win && win->renderer_) {
			if (win->renderer_->is_transparent_pixel(pt)) {
				return HTTRANSPARENT;
			}
		}

		return HTCLIENT;
	}
	case WM_LBUTTONDOWN:
	{
		if (win) {
			win->invoke(win->on_mouse_click);
		}
		return 0;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
