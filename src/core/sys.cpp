#include <core/sys.h>

#include <Windows.h>

int sys::display_width() {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	return static_cast<int>(desktop.right);
}
int sys::display_height() {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	return static_cast<int>(desktop.bottom);
}
