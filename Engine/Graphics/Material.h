#pragma once

#include "Math/Vector.h"
#include "Graphics/Buffer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"


class Material
{
public:
    bool Create(GraphicsDevice& device, Shader& shader, const char *diffuseTextPath);
    bool Bind(ID3D11DeviceContext* context);
private:
    struct MaterialConstant {
        Vector3 diffuseColor {0.6f, 0.6f, 0.6f};
        float specularPower = 64.0f;
        Vector3 specularColor {1.0f, 1.0f, 1.0f};
        float padding = 0.0f;
    };
    static_assert(sizeof(MaterialConstant) == 32);

    Shader* mShader = nullptr;
    Texture2D mDiffuseTexture;
    MaterialConstant mConstants;
    ConstantBuffer<MaterialConstant> mConstantBuffer;

    ComPtr<ID3D11SamplerState> mSamplerState;
};