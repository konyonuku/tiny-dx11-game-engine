#include "RenderState.h"

#include "Core/Log.h"
#include "Graphics/GraphicsDevice.h"

bool RenderState::Create(GraphicsDevice& device)
{
    D3D11_RASTERIZER_DESC rasterDesc{};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    // rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = FALSE;
    rasterDesc.DepthClipEnable = TRUE;

    HR_CHECK(device.Device()->CreateRasterizerState(&rasterDesc, mRasterizerState.GetAddressOf()));

    D3D11_DEPTH_STENCIL_DESC depthDesc{};
    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDesc.StencilEnable = FALSE;

    HR_CHECK(device.Device()->CreateDepthStencilState(&depthDesc, mDepthStencilState.GetAddressOf()));

    return true;
}

void RenderState::Bind(ID3D11DeviceContext* context) const
{
    context->RSSetState(mRasterizerState.Get());
    context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
}
