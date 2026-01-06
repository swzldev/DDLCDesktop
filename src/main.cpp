#include <iostream>
#include <string>
#include <exception>
#include <filesystem>
#include <cstdint>
#include <Windows.h>
#include <DbgHelp.h>

#include <core/widget.h>
#include <output/log.h>

#pragma comment(lib, "Dbghelp.lib")

LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* exceptionInfo) {
	log::print("FATAL ERROR: Unhandled exception caught!\n");
	log::print("Exception Code: 0x{:X}\n", exceptionInfo->ExceptionRecord->ExceptionCode);
	log::print("Exception Address: 0x{:X}\n", (uintptr_t)exceptionInfo->ExceptionRecord->ExceptionAddress);

	// create minidump
	HANDLE hFile = CreateFileA("crash_dump.dmp", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile != INVALID_HANDLE_VALUE) {
		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
		dumpInfo.ThreadId = GetCurrentThreadId();
		dumpInfo.ExceptionPointers = exceptionInfo;
		dumpInfo.ClientPointers = FALSE;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &dumpInfo, nullptr, nullptr);
		CloseHandle(hFile);
		log::print("Crash dump written to crash_dump.dmp\n");
	}

	log::shutdown();
	MessageBoxA(nullptr, "The application has crashed. Check log.txt for details.", "Fatal Error", MB_OK | MB_ICONERROR);

	return EXCEPTION_EXECUTE_HANDLER;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	SetUnhandledExceptionFilter(UnhandledExceptionHandler);

	std::filesystem::path log_path = std::filesystem::current_path() / "log.txt";

	try {
		log::open_file(log_path.string().c_str());
		widget::get_instance().main_loop();
	}
	catch (const std::exception& e) {
		log::print("FATAL ERROR: Unhandled exception: {}\n", e.what());
		log::shutdown();
		MessageBoxA(nullptr, e.what(), "Fatal Error", MB_OK | MB_ICONERROR);
		return 1;
	}
	catch (...) {
		log::print("FATAL ERROR: Unknown unhandled exception occurred.\n");
		log::shutdown();
		MessageBoxA(nullptr, "An unknown fatal error occurred.", "Fatal Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	log::shutdown();
	return 0;
}