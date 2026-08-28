#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include "Graphics/Buffer.h"
#include "Math/Matrix.h"


class TriangleRenderer
{
public:
    bool Create(GraphicsDevice& device);
    void Render(ID3D11DeviceContext* context, const Matrix4x4& world);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  mPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  mInputLayout;
    VertexBuffer                               mVertexBuffer;
    IndexBuffer                                mIndexBuffer;
    
    struct ObjectConstant
    {
        Matrix4x4 world;
    };
    ConstantBuffer<ObjectConstant>             mConstantBuffer;
};
