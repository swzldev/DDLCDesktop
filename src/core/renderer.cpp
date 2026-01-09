#include <core/renderer.h>

#include <stdexcept>
#include <d2d1.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dwrite.h>
#include <dwrite_3.h>
#include <wrl/client.h>

#include <core/sys.h>

renderer::renderer(HWND hwnd, int width, int height) {
    HRESULT hr = S_OK;

    // create D3D11 device and context
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &d3d_device_,
        nullptr,
        &d3d_ctx_
    );

    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create D3D11 device");
    }

    // get DXGI device
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;
    hr = d3d_device_.As(&dxgi_device);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to get DXGI device");
    }

    // create D2D factory
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2d_factory_.GetAddressOf());
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create D2D factory");
    }

    // create D2D device
    hr = d2d_factory_->CreateDevice(dxgi_device.Get(), &d2d_device_);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create D2D device");
    }

    // create D2D device context
    hr = d2d_device_->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        &d2d_ctx_
    );
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create D2D device context");
    }

    // create dwrite factory
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(dwrite_factory_.GetAddressOf())
	);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create DWrite factory");
	}

    // create text format
    hr = dwrite_factory_->CreateTextFormat(
        L"Aller",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        16.0f,
        L"en-us",
        &dwrite_text_format_
	);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create text format");
    }

	// set text alignment
    dwrite_text_format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    dwrite_text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    // create composition device
    hr = DCompositionCreateDevice(
        dxgi_device.Get(),
        __uuidof(IDCompositionDevice),
        reinterpret_cast<void**>(dcomp_device_.GetAddressOf())
    );
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create DirectComposition device");
    }

    // create composition target for the window
    hr = dcomp_device_->CreateTargetForHwnd(hwnd, TRUE, &dcomp_target_);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create composition target");
    }

    // create visual
    hr = dcomp_device_->CreateVisual(&dcomp_visual_);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create visual");
    }

    // set visual as root
    hr = dcomp_target_->SetRoot(dcomp_visual_.Get());
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to set root visual");
    }

    // store dimensions
    width_ = width;
    height_ = height;

    // create render target
    create_render_target();

    // commit composition
    hr = dcomp_device_->Commit();
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to commit composition");
    }
}

void renderer::resize(int width, int height) {
    width_ = width;
    height_ = height;

    if (d2d_brush_) {
        d2d_brush_.Reset();
    }
    if (d2d_target_bitmap_) {
        d2d_target_bitmap_.Reset();
    }
    if (swapchain_) {
        swapchain_.Reset();
    }

	create_render_target();

    HRESULT hr = dcomp_device_->Commit();
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to commit composition after resize");
    }
}

void renderer::begin_draw() {
    d2d_ctx_->BeginDraw();

    // clear with transparency
    d2d_ctx_->Clear(D2D1::ColorF(0, 0, 0, 0));
}
void renderer::end_draw() {
    HRESULT hr = d2d_ctx_->EndDraw();
    if (FAILED(hr)) {
        // handle device lost
        if (hr == D2DERR_RECREATE_TARGET) {
            create_render_target();
        }
    }

    // present
    hr = swapchain_->Present(1, 0);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to present swap chain");
    }

    // commit composition changes
    hr = dcomp_device_->Commit();
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to commit composition");
    }
}

void renderer::draw_sprite(sprite* spr, float x, float y) {
    if (!spr) {
        return;
    }

	float ax = x * width_;
	float ay = y * width_;

    ID2D1Bitmap* bitmap = spr->create_d2d_bitmap(d2d_ctx_.Get());
    if (!bitmap) {
        return;
    }
    D2D1_SIZE_F bmp_size = bitmap->GetSize();

    // handle rescaling
    float scale = static_cast<float>(width_) / bmp_size.width;
    float scaled_width = bmp_size.width * scale;
    float scaled_height = bmp_size.height * scale;

    D2D1_RECT_F dest_rect = D2D1::RectF(
        static_cast<float>(ax),
        static_cast<float>(ay),
        static_cast<float>(ax) + scaled_width,
        static_cast<float>(ay) + scaled_height
    );
    d2d_ctx_->DrawBitmap(
        bitmap,
        dest_rect,
        1.0f,
        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
        nullptr
    );
}
void renderer::draw_text(const std::wstring& text, float x, float y, float width, float height, int size, float outline) {
    if (!d2d_brush_) {
        return;
    }

	// create text format
    if (!create_text_format({ L"Aller", L"Segoe UI", L"Arial" }, size)) {
        return;
	}

    float sf = height_;

	x *= sf;
	y *= sf;

	width *= sf;
	height *= sf;

    // set brush color
    d2d_brush_->SetColor(D2D1::ColorF(D2D1::ColorF::White));

    // center the container box around x, y
    D2D1_RECT_F layout_rect = D2D1::RectF(
        x - width / 2.0f,
        y - height / 2.0f,
        x + width / 2.0f,
        y + height / 2.0f
    );

    // stroke
    if (outline > 0.0f) {
        d2d_brush_->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
        float outline_thickness = outline * (sf / sys::display_width());
        const int samples = 16;

        for (int i = 0; i < samples; i++) {
            float angle = (2.0f * 3.14159f * i) / samples;
            float ox = cosf(angle) * outline_thickness;
            float oy = sinf(angle) * outline_thickness;

            D2D1_RECT_F outline_rect = D2D1::RectF(
                layout_rect.left + ox,
                layout_rect.top + oy,
                layout_rect.right + ox,
                layout_rect.bottom + oy
            );

            d2d_ctx_->DrawTextW(
                text.c_str(),
                static_cast<UINT32>(text.length()),
                dwrite_text_format_.Get(),
                &outline_rect,
                d2d_brush_.Get()
            );
        }
    }

    // main text
    d2d_brush_->SetColor(D2D1::ColorF(D2D1::ColorF::White));
    d2d_ctx_->DrawTextW(
        text.c_str(),
        static_cast<UINT32>(text.length()),
        dwrite_text_format_.Get(),
        &layout_rect,
        d2d_brush_.Get()
    );
}

bool renderer::is_transparent_pixel(POINT pt) const {
	return false; // temporary
}

void renderer::create_render_target() {
    HRESULT hr = S_OK;

    // create swap chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = width_;
    swapChainDesc.Height = height_;
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    swapChainDesc.Flags = 0;

    Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;
    hr = d3d_device_.As(&dxgi_device);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to get DXGI device");
    }

    Microsoft::WRL::ComPtr<IDXGIAdapter> dxgi_adapter;
    hr = dxgi_device->GetAdapter(&dxgi_adapter);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to get DXGI adapter");
    }

    Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory;
    hr = dxgi_adapter->GetParent(__uuidof(IDXGIFactory2), &dxgi_factory);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to get DXGI factory");
    }

    hr = dxgi_factory->CreateSwapChainForComposition(
        dxgi_device.Get(),
        &swapChainDesc,
        nullptr,
        &swapchain_
    );
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create swap chain");
    }

    // get back buffer surface
    Microsoft::WRL::ComPtr<IDXGISurface> surface;
    hr = swapchain_->GetBuffer(0, __uuidof(IDXGISurface), &surface);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to get back buffer");
    }

    // create bitmap from surface
    D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );

    hr = d2d_ctx_->CreateBitmapFromDxgiSurface(
        surface.Get(),
        &bitmapProperties,
        &d2d_target_bitmap_
    );
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create D2D bitmap");
    }

    // set target bitmap
    d2d_ctx_->SetTarget(d2d_target_bitmap_.Get());

    // create text brush
    hr = d2d_ctx_->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        &d2d_brush_
    );
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create brush");
    }

    // set swap chain as visual content
    hr = dcomp_visual_->SetContent(swapchain_.Get());
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to set visual content");
    }
}

bool renderer::create_text_format(const std::vector<std::wstring>& font_families, int em) {
    float base_font_size = height_ / 100.0f;
	float font_size = base_font_size * em;

    for (const auto& family : font_families) {
        HRESULT hr = dwrite_factory_->CreateTextFormat(
            family.c_str(),
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            font_size,
            L"en-us",
            &dwrite_text_format_
        );
        if (SUCCEEDED(hr)) {
            return true;
        }
	}
	return false;
}
