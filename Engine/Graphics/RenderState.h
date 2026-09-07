#pragma once
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class GraphicsDevice;

class RenderState {
public:
    bool Create(GraphicsDevice& device);
    void Bind(ID3D11DeviceContext* context) const;

private:
    ComPtr<ID3D11RasterizerState> mRasterizerState;
    ComPtr<ID3D11DepthStencilState> mDepthStencilState;
};