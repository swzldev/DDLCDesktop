#pragma once

#include <string>
#include <functional>

class input {
public:
	static bool is_key_down(int vkey);
	static bool is_key_pressed(int vkey);
	static bool is_key_released(int vkey);

	static void tick();

	static void begin_input_recording(std::string* buffer, int max_length = 50, std::function<void()> on_submit = nullptr);
	static void end_input_recording();

private:
	friend class window;

	static void on_char_input(wchar_t c);

	static bool key_states_[256];
	static bool prev_key_states_[256];

	static std::function<void()> on_submit_;
	static std::string* cur_input_buffer_;
	static int max_input_length_;
};