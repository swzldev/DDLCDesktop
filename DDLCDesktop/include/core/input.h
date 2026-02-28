#pragma once

#include <string>
#include <functional>

class input {
public:
	static bool is_key_down(int vkey);
	static bool is_key_pressed(int vkey);
	static bool is_key_released(int vkey);

	static void tick();

	static void begin_input_recording(const std::function<void(wchar_t)>& recorder);
	static void end_input_recording();

	static std::string get_clipboard_text();
	static void set_clipboard_text(const std::string& text);

private:
	friend class window;

	static void on_char_input(wchar_t c);

	static bool key_states_[256];
	static bool prev_key_states_[256];

	static std::function<void(wchar_t)> input_recorder_;
};