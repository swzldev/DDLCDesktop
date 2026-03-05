#include <error/internal.h>

#include <Windows.h>
#include <string>

void d_panic(const std::string& message) {
	MessageBoxA(NULL, message.c_str(), "Fatal Runtime Error", MB_ICONERROR | MB_OK);
	exit(1);
}
