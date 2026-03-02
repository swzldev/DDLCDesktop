#pragma once

#include <string>
#include <unordered_map>

#include <d2d1.h>
#include <d2d1_1.h>

class sprite {
public:
	~sprite();

	static sprite* load_from_file(const std::string& filepath);
	static void cleanup_all_sprites();

	ID2D1Bitmap* create_d2d_bitmap(ID2D1DeviceContext* d2d_ctx);
	void release_d2d_bitmap();

private:
	sprite() = default;

	static std::unordered_map<std::string, sprite*> sprite_cache_;

	std::string filepath_;
	int width_ = 0;
	int height_ = 0;
	unsigned char* data_ = nullptr;

	ID2D1Bitmap* cached_bitmap_ = nullptr;
	ID2D1DeviceContext* cached_ctx_ = nullptr;
};
