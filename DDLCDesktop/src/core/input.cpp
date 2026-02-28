#include <core/input.h>

#include <string>
#include <functional>
#include <Windows.h>

bool input::is_key_down(int vkey) {
	return key_states_[vkey];
}
bool input::is_key_pressed(int vkey) {
	return key_states_[vkey] && !prev_key_states_[vkey];
}
bool input::is_key_released(int vkey) {
	return !key_states_[vkey] && prev_key_states_[vkey];
}

void input::tick() {
	for (int i = 0; i < 256; i++) {
		prev_key_states_[i] = key_states_[i];
		key_states_[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
	}
}

void input::begin_input_recording(const std::function<void(wchar_t)>& recorder) {
	input_recorder_ = recorder;
}
void input::end_input_recording() {
	input_recorder_ = nullptr;
}

std::string input::get_clipboard_text() {
	OpenClipboard(nullptr);
	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData == nullptr) {
		CloseClipboard();
		return std::string();
	}

	char* pszText = static_cast<char*>(GlobalLock(hData));
	if (pszText == nullptr) {
		CloseClipboard();
		return std::string();
	}

	std::string text(pszText);
	GlobalUnlock(hData);
	CloseClipboard();
	return text;
}
void input::set_clipboard_text(const std::string& text) {
	OpenClipboard(nullptr);
	EmptyClipboard();
	HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
	if (hGlob != nullptr) {
		char* pGlob = static_cast<char*>(GlobalLock(hGlob));
		if (pGlob != nullptr) {
			memcpy(pGlob, text.c_str(), text.size() + 1);
			GlobalUnlock(hGlob);
			SetClipboardData(CF_TEXT, hGlob);
		}
		else {
			GlobalFree(hGlob);
		}
	}
	CloseClipboard();
}

void input::on_char_input(wchar_t c) {
	if (input_recorder_) {
		input_recorder_(c);
	}
}

bool input::key_states_[256] = { false };
bool input::prev_key_states_[256] = { false };

std::function<void(wchar_t)> input::input_recorder_ = nullptr;
