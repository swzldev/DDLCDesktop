#include <core/input.h>

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

bool input::key_states_[256] = { false };
bool input::prev_key_states_[256] = { false };
