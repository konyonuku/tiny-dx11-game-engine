#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include "Graphics/Buffer.h"

class GraphicsDevice;

class TriangleRenderer
{
public:
    bool Create(GraphicsDevice& device);
    void Render(ID3D11DeviceContext* context);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  mPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  mInputLayout;
    VertexBuffer                               mVertexBuffer;
};
