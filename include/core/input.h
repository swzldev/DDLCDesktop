#pragma once

// basic wrapper around GetAsyncKeyState

class input {
public:
	static bool is_key_down(int vkey);
	static bool is_key_pressed(int vkey);
	static bool is_key_released(int vkey);

	static void tick();

private:
	static bool key_states_[256];
	static bool prev_key_states_[256];
};