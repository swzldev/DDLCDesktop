#include <core/noticon.h>

#include <string>
#include <utility>

#include <core/window.h>
#include <error/internal.h>

noticon::~noticon() {
	dispose();
}

void noticon::initialize(const wchar_t* tooltip) {
	HINSTANCE hInstance = GetModuleHandleW(NULL);

	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.lpfnWndProc = noticon::wnd_proc;
	wc.hInstance = hInstance;
	wc.lpszClassName = WNDCLASS_NAME;

	if (!RegisterClassExW(&wc)) {
		if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
			d_panic("Failed to register noticon window class!");
		}
	}

	msg_hwnd_ = CreateWindowExW(
		0,
		WNDCLASS_NAME,
		L"",
		WS_OVERLAPPED,
		0, 0, 0, 0,
		NULL, NULL, hInstance, this
	);

	if (!msg_hwnd_) {
		d_panic("Failed to create noticon message window!");
	}

	if (!tray_menu_) {
		tray_menu_ = CreatePopupMenu();
	}

	tray_icon_ = {};
	tray_icon_.cbSize = sizeof(tray_icon_);
	tray_icon_.hWnd = msg_hwnd_;
	tray_icon_.uID = TRAY_ICON_ID;
	tray_icon_.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
	tray_icon_.uCallbackMessage = WM_TRAYICON;
	tray_icon_.hIcon = LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(101));
	wcscpy_s(tray_icon_.szTip, tooltip);

	tray_added_ = (Shell_NotifyIconW(NIM_ADD, &tray_icon_) == TRUE);
	if (tray_added_) {
		tray_icon_.uVersion = NOTIFYICON_VERSION_4;
		Shell_NotifyIconW(NIM_SETVERSION, &tray_icon_);
	}
}
void noticon::dispose() {
	if (tray_added_) {
		Shell_NotifyIconW(NIM_DELETE, &tray_icon_);
		tray_added_ = false;
	}

	if (msg_hwnd_) {
		DestroyWindow(msg_hwnd_);
		msg_hwnd_ = NULL;
	}

	if (tray_menu_) {
		DestroyMenu(tray_menu_);
		tray_menu_ = NULL;
	}

	callbacks_.clear();
}

void noticon::add_button(const std::wstring& display_name, std::function<void()> callback) {
	if (tray_menu_ == NULL) {
		d_panic("Tray menu is null!");
	}

	const UINT id = next_dynamic_id_++;

	AppendMenuW(tray_menu_, MF_STRING, id, display_name.c_str());
	callbacks_[id] = std::move(callback);
}
void noticon::add_separator() const {
	if (tray_menu_ == NULL) {
		d_panic("Tray menu is null!");
	}

	AppendMenuW(tray_menu_, MF_SEPARATOR, 0, NULL);
}

bool noticon::handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_TRAYICON) {
		const UINT notification = LOWORD(lParam);
		const UINT icon_id = HIWORD(lParam);

		if (icon_id != TRAY_ICON_ID) {
			return false;
		}

		switch (notification) {
		case WM_LBUTTONDBLCLK:
		case NIN_SELECT:
		case NIN_KEYSELECT:
			if (on_double_click_) {
				on_double_click_();
			}
			return true;
		case WM_RBUTTONUP:
		case WM_CONTEXTMENU:
			show_context_menu();
			return true;
		}
		return false;
	}

	if (uMsg == WM_COMMAND) {
		const UINT id = LOWORD(wParam);
		auto it = callbacks_.find(id);
		if (it != callbacks_.end()) {
			it->second();
			return true;
		}
	}

	return false;
}

void noticon::show_context_menu() const {
	if (!tray_menu_ || !msg_hwnd_) {
		return;
	}

	POINT pt = {};
	GetCursorPos(&pt);

	SetForegroundWindow(msg_hwnd_);
	TrackPopupMenu(
		tray_menu_,
		TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_BOTTOMALIGN,
		pt.x,
		pt.y,
		0,
		msg_hwnd_,
		NULL
	);

	PostMessageW(msg_hwnd_, WM_NULL, 0, 0);
}

LRESULT noticon::wnd_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_NCCREATE) {
		auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
		return TRUE;
	}

	auto* self = reinterpret_cast<noticon*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	if (!self) {
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}

	if (self->handle_message(uMsg, wParam, lParam)) {
		return 0;
	}

	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
