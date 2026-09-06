#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include "Graphics/Buffer.h"
#include "Graphics/Texture2D.h"
#include "Math/Matrix.h"


class TriangleRenderer
{
public:
    bool Create(GraphicsDevice& device);
    void Render(ID3D11DeviceContext* context, const Matrix4x4& world, const Matrix4x4& wvp, const Vector3& camera);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  mPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  mInputLayout;
    VertexBuffer                               mVertexBuffer;
    IndexBuffer                                mIndexBuffer;
    struct ObjectConstant
    {
        Matrix4x4 wvp;
        Matrix4x4 world; //for normal
    };
    struct LightConstant
    {
        Vector3 lightDirection; 
        float   lightIntensity;
        Vector3 lightColor;
        float   ambientIntensity;
        Vector3 cameraPosition;
        float   padding; //12+12+4+4+12 32 44 need4
    };
    static_assert(sizeof(ObjectConstant) % 16 == 0);
    static_assert(sizeof(LightConstant) % 16 == 0);

    ConstantBuffer<ObjectConstant>             mObjectConstantBuffer;
    ConstantBuffer<LightConstant>              mLightConstantBuffer;

    Texture2D                                  mTexture;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerState;    
};
