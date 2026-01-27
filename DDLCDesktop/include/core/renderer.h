#pragma once

#include <vector>
#include <string>
#include <functional>
#include <cstdint>
#include <Windows.h>
#include <dcomp.h>
#include <d3d11.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>

#include <visual/sprite.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dcomp.lib")
#pragma comment(lib, "dwrite.lib")

using namespace Microsoft::WRL;

class window;

class renderer {
public:
	renderer(window* wnd, HWND hwnd, int width, int height);
	~renderer() = default;

	inline window* get_window() const { return window_; }

	void resize(int width, int height);

	void begin_draw();
	void end_draw();

	void set_text_color(D2D_COLOR_F color);
	void set_stroke_color(D2D_COLOR_F color);
	void set_text_alignment(DWRITE_TEXT_ALIGNMENT alignment);

	void draw_sprite(sprite* spr, float x = 0.0f, float y = 0.0f);
	void draw_sprite(sprite* spr, float x, float y, float width, float height);
	void draw_text(const std::wstring& text, float x, float y, float width, float height, float size = 4.0f, float outline = 0.0f);

	std::vector<uint8_t> get_alpha_map();

	D2D1_SIZE_F measure_text(const std::wstring& text, float size = 4.0f);

	std::vector<std::function<void()>> post_draw_queue;

private:
	window* window_;

	ComPtr<ID3D11Device> d3d_device_;
	ComPtr<ID3D11DeviceContext> d3d_ctx_;

	ComPtr<ID2D1Factory1> d2d_factory_;
	ComPtr<ID2D1Device> d2d_device_;
	ComPtr<ID2D1DeviceContext> d2d_ctx_;
	ComPtr<ID2D1Bitmap1> d2d_target_bitmap_;
	ComPtr<ID2D1SolidColorBrush> d2d_brush_;

	ComPtr<IDXGISwapChain1> swapchain_;

	ComPtr<IDCompositionDevice> dcomp_device_;
	ComPtr<IDCompositionTarget> dcomp_target_;
	ComPtr<IDCompositionVisual> dcomp_visual_;

	ComPtr<IDWriteFactory> dwrite_factory_;
	ComPtr<IDWriteTextFormat> dwrite_text_format_;

	ComPtr<ID3D11Texture2D> cached_frame_;

	DWRITE_TEXT_ALIGNMENT text_alignment_ = DWRITE_TEXT_ALIGNMENT_CENTER;

	D2D_COLOR_F text_color_ = D2D1::ColorF(D2D1::ColorF::White);
	D2D_COLOR_F stroke_color_ = D2D1::ColorF(D2D1::ColorF::Black);

	int width_;
	int height_;

	void create_render_target();

	bool create_text_format(const std::vector<std::wstring>& font_families, float size);
};
