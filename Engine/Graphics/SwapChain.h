#pragma once
#include <cstdint>

#include "Graphics/GraphicsDevice.h"

class SwapChain
{
public:
    bool Create(GraphicsDevice& device, HWND hwnd, uint32_t width, uint32_t height);
    bool Resize(uint32_t width, uint32_t height);
    void Present(bool vsync);

    ID3D11RenderTargetView* RenderTargetView() const { return mRTV.Get(); }
    ID3D11DepthStencilView* DepthStencilView() const { return mDSV.Get(); }
    const D3D11_VIEWPORT&   Viewport()         const { return mViewport; }

private:
    bool CreateBuffers(uint32_t width, uint32_t height);
    void ReleaseBuffers();

    GraphicsDevice* mDevice = nullptr;

    ComPtr<IDXGISwapChain1>        mSwapChain;
    ComPtr<ID3D11RenderTargetView> mRTV;
    ComPtr<ID3D11Texture2D>        mDepthBuffer;
    ComPtr<ID3D11DepthStencilView> mDSV;

    D3D11_VIEWPORT mViewport {};
    bool           mAllowTearing = false;
};
