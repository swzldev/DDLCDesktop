#include <error/internal.h>

#include <Windows.h>

void d_panic(const char* message) {
	MessageBoxA(NULL, message, "Fatal Runtime Error", MB_ICONERROR | MB_OK);
	exit(1);
}
