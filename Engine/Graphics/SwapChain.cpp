#include "Graphics/SwapChain.h"

#include "Core/Log.h"

bool SwapChain::Create(GraphicsDevice& device, HWND hwnd, uint32_t width, uint32_t height)
{
    mDevice       = &device;
    mAllowTearing = device.AllowTearing();

    DXGI_SWAP_CHAIN_DESC1 sd {};
    sd.Width       = width;
    sd.Height      = height;
    sd.Format      = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.SampleDesc  = { 1, 0 };
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2;
    sd.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags       = mAllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    HR_CHECK(device.Factory()->CreateSwapChainForHwnd(
        device.Device(), hwnd, &sd, nullptr, nullptr, &mSwapChain));

    device.Factory()->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

    Core::LogInfo("SwapChain created. %ux%u flip-discard", width, height);
    return CreateBuffers(width, height);
}

bool SwapChain::CreateBuffers(uint32_t width, uint32_t height)
{
    ID3D11Device* device = mDevice->Device();

    ComPtr<ID3D11Texture2D> backBuffer;
    HR_CHECK(mSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
    HR_CHECK(device->CreateRenderTargetView(backBuffer.Get(), nullptr, &mRTV));

    D3D11_TEXTURE2D_DESC depthDesc {};
    depthDesc.Width      = width;
    depthDesc.Height     = height;
    depthDesc.MipLevels  = 1;
    depthDesc.ArraySize  = 1;
    depthDesc.Format     = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc = { 1, 0 };
    depthDesc.Usage      = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags  = D3D11_BIND_DEPTH_STENCIL;

    HR_CHECK(device->CreateTexture2D(&depthDesc, nullptr, &mDepthBuffer));
    HR_CHECK(device->CreateDepthStencilView(mDepthBuffer.Get(), nullptr, &mDSV));

    mViewport = { 0.f, 0.f, static_cast<float>(width), static_cast<float>(height), 0.f, 1.f };
    return true;
}

void SwapChain::ReleaseBuffers()
{
    mRTV.Reset();
    mDSV.Reset();
    mDepthBuffer.Reset();
}

bool SwapChain::Resize(uint32_t width, uint32_t height)
{
    if (!mSwapChain || width == 0 || height == 0) return true;

    mDevice->Context()->OMSetRenderTargets(0, nullptr, nullptr);
    ReleaseBuffers();

    const UINT flags = mAllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
    HR_CHECK(mSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, flags));

    Core::LogInfo("SwapChain resized. %ux%u", width, height);
    return CreateBuffers(width, height);
}

void SwapChain::Present(bool vsync)
{
    const UINT flags = (!vsync && mAllowTearing) ? DXGI_PRESENT_ALLOW_TEARING : 0;
    mSwapChain->Present(vsync ? 1 : 0, flags);
}
