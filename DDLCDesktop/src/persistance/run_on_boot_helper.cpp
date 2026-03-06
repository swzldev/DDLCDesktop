#include <persistance/run_on_boot_helper.h>

#include <Windows.h>
#include <string>

static const wchar_t* RUN_KEY = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static const wchar_t* APP_NAME = L"DDLCDesktop";

bool run_on_boot_helper::enable_run_on_boot() {
    wchar_t exe_path[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, exe_path, MAX_PATH))
        return false;

    HKEY key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, RUN_KEY, 0, KEY_SET_VALUE, &key) != ERROR_SUCCESS)
        return false;

    // Wrap path in quotes to handle spaces
    std::wstring path = L"\"" + std::wstring(exe_path) + L"\"";
    bool success = RegSetValueExW(
        key, APP_NAME, 0, REG_SZ,
        reinterpret_cast<const BYTE*>(path.c_str()),
        static_cast<DWORD>((path.size() + 1) * sizeof(wchar_t))
    ) == ERROR_SUCCESS;

    RegCloseKey(key);
    return success;
}
bool run_on_boot_helper::disable_run_on_boot() {
    HKEY key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, RUN_KEY, 0, KEY_SET_VALUE, &key) != ERROR_SUCCESS)
        return false;

    bool success = RegDeleteValueW(key, APP_NAME) == ERROR_SUCCESS;
    RegCloseKey(key);
    return success;
}
