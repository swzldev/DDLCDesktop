#pragma once

class bounds {
public:
	bounds(float top, float bottom, float left, float right)
		: top(top), bottom(bottom), left(left), right(right) {
	}

	inline bool contains(float x, float y) const {
		return x >= left && x <= right && y >= top && y <= bottom;
	}

	float top, bottom, left, right;
};
