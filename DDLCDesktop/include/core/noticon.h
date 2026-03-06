#pragma once

#include <string>
#include <functional>

#include <Windows.h>
#include <shellapi.h>

class noticon {
public:
	noticon() = default;
	~noticon();

	void initialize(const wchar_t* tooltip);
	void dispose();

	inline void set_on_double_click(std::function<void()> callback) {
		on_double_click_ = std::move(callback);
	}

	void add_button(const std::wstring& display_name, std::function<void()> callback);
	void add_separator() const;

	bool handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	static constexpr UINT WM_TRAYICON = WM_APP + 1;
	static constexpr UINT TRAY_ICON_ID = 1;
	static constexpr UINT ID_DYNAMIC_FIRST = 1001;
	static constexpr const wchar_t* WNDCLASS_NAME = L"DDLCDesktopNoticonMessageWindow";

private:
	HWND msg_hwnd_ = NULL;
	NOTIFYICONDATAW tray_icon_ = {};
	HMENU tray_menu_ = NULL;
	bool tray_added_ = false;

	UINT next_dynamic_id_ = ID_DYNAMIC_FIRST;
	std::function<void()> on_double_click_;
	std::unordered_map<UINT, std::function<void()>> callbacks_;

	void show_context_menu() const;

	static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
