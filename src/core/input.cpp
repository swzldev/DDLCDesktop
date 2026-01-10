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

void input::begin_input_recording(std::string* buffer, std::function<void()> on_submit) {
	cur_input_buffer_ = buffer;
	on_submit_ = on_submit;
}
void input::end_input_recording() {
	cur_input_buffer_ = nullptr;
}

void input::on_char_input(wchar_t c) {
	if (cur_input_buffer_) {
		// backspace
		if (c == VK_BACK) {
			if (!cur_input_buffer_->empty()) {
				cur_input_buffer_->pop_back();
			}
		}
		// submit (enter)
		else if (c == VK_RETURN) {
			if (on_submit_) {
				on_submit_();
			}
		}

		// printable (ASCII) characters
		else if (c >= 32 && c <= 126) {
			cur_input_buffer_->push_back(static_cast<char>(c));
		}
	}
}

bool input::key_states_[256] = { false };
bool input::prev_key_states_[256] = { false };

std::function<void()> input::on_submit_ = nullptr;
std::string* input::cur_input_buffer_ = nullptr;
