#pragma once

#include <string>
#include <functional>

#include <Windows.h>
#include <shellapi.h>

class window;

class noticon {
public:
	noticon() = default;
	~noticon();

	void initialize(window* owner, const wchar_t* tooltip);
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

private:
	HWND owner_ = NULL;
	NOTIFYICONDATAW tray_icon_ = {};
	HMENU tray_menu_ = NULL;
	bool tray_added_ = false;

	UINT next_dynamic_id_ = ID_DYNAMIC_FIRST;
	std::function<void()> on_double_click_;
	std::unordered_map<UINT, std::function<void()>> callbacks_;

	void show_context_menu() const;
};
