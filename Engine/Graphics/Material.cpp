#include "Material.h"

#include "Core/Log.h"


bool Material::Create(GraphicsDevice &device, Shader& shader, const char *diffuseTextPath)
{
    mShader = &shader;

    if(!mDiffuseTexture.CreateFromFile(device, diffuseTextPath))
        return false;

    if(!mConstantBuffer.Create(device))
        return false;


    D3D11_SAMPLER_DESC desc{};
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.MaxAnisotropy = 1;
    desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    desc.MinLOD = 0.0f;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    HR_CHECK(device.Device()->CreateSamplerState(&desc, mSamplerState.GetAddressOf()));


    return true;
}

bool Material::Bind(ID3D11DeviceContext *context)
{
    if(mShader == nullptr)
        return false;

    mShader->Bind(context);

    if(!mConstantBuffer.Update(context, mConstants))
        return false;

    mConstantBuffer.BindPS(context, 2); //b2
    mDiffuseTexture.BindPS(context, 0); //t0

    ID3D11SamplerState* sampler = mSamplerState.Get();
    context->PSSetSamplers(0, 1, &sampler); //s0


    return true;
}
