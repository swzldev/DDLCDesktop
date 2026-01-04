#include <iostream>
#include <Windows.h>

#include <core/widget.h>
#include <output/log.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	log::open_file("log.txt");

	widget main_widget;
	main_widget.main_loop();

	log::shutdown();
	return 0;
}