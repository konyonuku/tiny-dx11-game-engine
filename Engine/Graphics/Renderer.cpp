#include "Graphics/Renderer.h"

#include "Graphics/GraphicsDevice.h"
#include "Graphics/SwapChain.h"

bool Renderer::Create(GraphicsDevice& device, SwapChain& swapChain)
{
    mDevice    = &device;
    mSwapChain = &swapChain;

    if(!mDefaultRenderState.Create(device))
        return false;

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
    ID3D11DeviceContext* context = mDevice->Context();
    ID3D11RenderTargetView* renderTargetView = mSwapChain->RenderTargetView();

    context->OMSetRenderTargets(1, &renderTargetView, mSwapChain->DepthStencilView());
    context->RSSetViewports(1, &mSwapChain->Viewport());

    mDefaultRenderState.Bind(context);

    context->ClearRenderTargetView(renderTargetView, mClearColor);
    context->ClearDepthStencilView(mSwapChain->DepthStencilView(),
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
