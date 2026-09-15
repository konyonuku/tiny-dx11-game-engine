#include "Graphics/Renderer.h"

#include "Graphics/GraphicsDevice.h"
#include "Graphics/SwapChain.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"


bool Renderer::Create(GraphicsDevice& device, SwapChain& swapChain)
{
    mDevice    = &device;
    mSwapChain = &swapChain;
    if(!mDefaultRenderState.Create(device)) return false;
    if(!mObjectConstantBuffer.Create(device)) return false;
    if(!mFrameConstantBuffer.Create(device)) return false;

    return true;
}

bool Renderer::DrawMesh(const Mesh &mesh, Material &material, const Matrix4x4 &world)
{
    if(!mFrameConstantsReady) return false;

    ObjectConstants objectConstants {};
    objectConstants.world = world.Transposed();

    ID3D11DeviceContext* context = Context();

    if(!mObjectConstantBuffer.Update(context, objectConstants)) return false;

    mObjectConstantBuffer.BindVS(context, 0);

    if(!material.Bind(context)) return false;

    mesh.Bind(context);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    mesh.Draw(context);

    return true;
}

bool Renderer::SetFrameConstants(const FrameConstants &constants)
{
    mFrameConstantsReady = false;

    FrameConstants gpuConstants = constants;
    gpuConstants.viewProjection = gpuConstants.viewProjection.Transposed();

    ID3D11DeviceContext* context = Context();
    
    if(!mFrameConstantBuffer.Update(context, gpuConstants)) return false;

    mFrameConstantBuffer.BindVS(context, 1);
    mFrameConstantBuffer.BindPS(context, 1);

    mFrameConstantsReady = true;

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
    mFrameConstantsReady = false;

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
