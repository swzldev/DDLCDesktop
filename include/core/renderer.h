#pragma once

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

class renderer {
public:
	renderer(HWND hwnd, int width, int height);
	~renderer() = default;

	void resize(int width, int height);

	void begin_draw();
	void end_draw();

	void draw_sprite(sprite* spr, int x = 0, int y = 0);
	void draw_text(const std::wstring& text, float x, float y, float width, float height, int em = 12);

	bool is_transparent_pixel(POINT pt) const;

private:
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

	int width_;
	int height_;

	void create_render_target();

	bool create_text_format(const std::vector<std::wstring>& font_families, int em);
};
