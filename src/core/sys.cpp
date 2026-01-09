#include <core/system.h>

#include <Windows.h>

int system::display_width() {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	return static_cast<int>(desktop.right);
}
int system::display_height() {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	return static_cast<int>(desktop.bottom);
}
