#pragma once

#include "Math/Vector.h"
#include "Graphics/Buffer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"


class Material
{
public:
    bool Create(GraphicsDevice& device, Shader& shader, const char *diffuseTextPath);
    bool BindPS(ID3D11DeviceContext* context);
private:
    struct MaterialConstant {
        Vector3 diffuseColor {1.0f, 1.0f, 1.0f};
        float specularPower = 32.0f;
        Vector3 specularColor {0.25f, 0.25f, 0.25f};
        float padding = 0.0f;
    };
    static_assert(sizeof(MaterialConstant) == 32);

    Shader* mShader = nullptr;
    Texture2D mDiffuseTexture;
    MaterialConstant mConstants;
    ConstantBuffer<MaterialConstant> mConstantBuffer;

    ComPtr<ID3D11SamplerState> mSamplerState;
};