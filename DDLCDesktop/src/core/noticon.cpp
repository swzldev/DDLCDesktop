#include <core/noticon.h>

#include <string>
#include <utility>

#include <core/window.h>
#include <error/internal.h>

noticon::~noticon() {
	dispose();
}

void noticon::initialize(window* owner, const wchar_t* tooltip) {
	owner_ = owner->get_handle();

	if (!owner_) {
		d_panic("owner HWND is null!");
	}

	if (!tray_menu_) {
		tray_menu_ = CreatePopupMenu();
	}

	tray_icon_ = {};
	tray_icon_.cbSize = sizeof(tray_icon_);
	tray_icon_.hWnd = owner_;
	tray_icon_.uID = TRAY_ICON_ID;
	tray_icon_.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
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

	if (tray_menu_) {
		DestroyMenu(tray_menu_);
		tray_menu_ = NULL;
	}

	callbacks_.clear();
	owner_ = NULL;
}

void noticon::add_button(const std::wstring& display_name, std::function<void()> callback) {
	if (!tray_menu_) {
		d_panic("Tray menu is null!");
	}

	const UINT id = next_dynamic_id_++;

	AppendMenuW(tray_menu_, MF_STRING, id, display_name.c_str());
	callbacks_[id] = std::move(callback);
}
void noticon::add_separator() const {
	if (!tray_menu_) {
		d_panic("Tray menu is null!");
	}

	AppendMenuW(tray_menu_, MF_SEPARATOR, 0, NULL);
}

bool noticon::handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_TRAYICON && wParam == TRAY_ICON_ID) {
		switch (static_cast<UINT>(lParam)) {
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
	if (!tray_menu_ || !owner_) {
		return;
	}

	POINT pt = {};
	GetCursorPos(&pt);

	SetForegroundWindow(owner_);
	TrackPopupMenu(
		tray_menu_,
		TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_BOTTOMALIGN,
		pt.x,
		pt.y,
		0,
		owner_,
		NULL
	);

	PostMessageW(owner_, WM_NULL, 0, 0);
}
