#include "Graphics/Renderer.h"

#include "Graphics/GraphicsDevice.h"
#include "Graphics/SwapChain.h"

bool Renderer::Create(GraphicsDevice& device, SwapChain& swapChain)
{
    mDevice    = &device;
    mSwapChain = &swapChain;
    return true;
}

void Renderer::SetClearColor(float r, float g, float b, float a)
{
    mClearColor[0] = r;
    mClearColor[1] = g;
    mClearColor[2] = b;
    mClearColor[3] = a;
}

void Renderer::BeginFrame()
{
    ID3D11DeviceContext* ctx = mDevice->Context();
    ID3D11RenderTargetView* rtv = mSwapChain->RenderTargetView();

    ctx->OMSetRenderTargets(1, &rtv, mSwapChain->DepthStencilView());
    ctx->RSSetViewports(1, &mSwapChain->Viewport());

    ctx->ClearRenderTargetView(rtv, mClearColor);
    ctx->ClearDepthStencilView(mSwapChain->DepthStencilView(),
                               D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void Renderer::EndFrame(bool vsync)
{
    mSwapChain->Present(vsync);
}

ID3D11DeviceContext* Renderer::Context() const
{
    return mDevice->Context();
}
