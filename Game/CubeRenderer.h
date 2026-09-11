#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include "Math/Matrix.h"
#include "Graphics/Buffer.h"
#include "Graphics/Shader.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/Texture2D.h"


class CubeRenderer
{
public:
    bool Create(GraphicsDevice& device);
    void Render(ID3D11DeviceContext* context, const Matrix4x4& world, const Matrix4x4& wvp, const Vector3& camera);

private:
    struct ObjectConstant
    {
        Matrix4x4 wvp;
        Matrix4x4 world;
    };
    struct LightConstant
    {
        Vector3 lightDirection; 
        float   lightIntensity;
        Vector3 lightColor;
        float   ambientIntensity;
        Vector3 cameraPosition;
        float   padding;
    };
    static_assert(sizeof(ObjectConstant) % 16 == 0);
    static_assert(sizeof(LightConstant) % 16 == 0);

    ConstantBuffer<ObjectConstant>             mObjectConstantBuffer;
    ConstantBuffer<LightConstant>              mLightConstantBuffer;

    Shader                                     mShader;
    Mesh                                       mMesh;
    Material                                   mMaterial;
};
