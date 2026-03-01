#include <visual/sprite.h>

#include <string>
#include <stdexcept>
#include <vector>
#include <d2d1.h>
#include <d2d1_1.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

sprite::~sprite() {
	// free image data if loaded
	if (data_) {
		stbi_image_free(data_);
		data_ = nullptr;
	}

	// release cached bitmap
	if (cached_bitmap_) {
		cached_bitmap_->Release();
		cached_bitmap_ = nullptr;
		cached_ctx_ = nullptr;
	}
}

sprite* sprite::load_from_file(const std::string& filepath) {
	// check if cached
	if (sprite_cache_.find(filepath) != sprite_cache_.end()) {
		return sprite_cache_[filepath];
	}

	sprite* spr = new sprite();

	int channels;
	unsigned char* data = stbi_load(filepath.c_str(), &spr->width_, &spr->height_, &channels, 4);
	if (!data) {
		throw std::runtime_error("Failed to load image: " + filepath);
	}

	spr->filepath_ = filepath;
	spr->data_ = data;

	// cache (also for cleanup)
	sprite_cache_[filepath] = spr;

	return spr;
}

void sprite::cleanup_all_sprites() {
	for (auto& pair : sprite_cache_) {
		if (pair.second) {
			delete pair.second;
			pair.second = nullptr;
		}
	}
	sprite_cache_.clear();
}

ID2D1Bitmap* sprite::create_d2d_bitmap(ID2D1DeviceContext* d2d_ctx) {
	if (!data_ || !d2d_ctx) {
		return nullptr;
	}

	// check cached bitmap
	if (cached_bitmap_ && cached_ctx_ == d2d_ctx) {
		return cached_bitmap_;
	}

	size_t wxh = static_cast<size_t>(width_) * static_cast<size_t>(height_);

	// convert rgba -> premult bgra
	std::vector<unsigned char> bgra_data(wxh * 4);

	for (size_t i = 0; i < wxh; ++i) {
		unsigned char r = data_[i * 4 + 0];
		unsigned char g = data_[i * 4 + 1];
		unsigned char b = data_[i * 4 + 2];
		unsigned char a = data_[i * 4 + 3];
		
		// premultiply alpha
		float alpha = a / 255.0f;
		bgra_data[i * 4 + 0] = static_cast<unsigned char>(b * alpha); // B
		bgra_data[i * 4 + 1] = static_cast<unsigned char>(g * alpha); // G
		bgra_data[i * 4 + 2] = static_cast<unsigned char>(r * alpha); // R
		bgra_data[i * 4 + 3] = a;                                     // A
	}

	// create props
	D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_NONE,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);

	// create bitmap
	ID2D1Bitmap1* bitmap = nullptr;
	HRESULT hr = d2d_ctx->CreateBitmap(
		D2D1::SizeU(width_, height_),
		bgra_data.data(),
		width_ * 4,
		&props,
		&bitmap
	);

	if (FAILED(hr)) {
		return nullptr;
	}

	// cache bitmap and context
	cached_bitmap_ = bitmap;
	cached_ctx_ = d2d_ctx;

	return bitmap;
}
void sprite::release_d2d_bitmap() {
	if (cached_bitmap_) {
		cached_bitmap_->Release();
		cached_bitmap_ = nullptr;
		cached_ctx_ = nullptr;
	}
}

std::unordered_map<std::string, sprite*> sprite::sprite_cache_ = {};
