#include "TriangleRenderer.h"

#include <cstddef>

#include "Core/Log.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ShaderCompiler.h"

namespace
{
    struct Vertex
    {
        float position[3];
        float normal[3];
        float uv[2];
    };

    const Vertex kVertices[] = {
        { { -0.5f, +0.5f, -0.5f }, { 0.f, 0.f, -1.f }, {0.f, 0.f}}, //0 - front
        { { +0.5f, +0.5f, -0.5f }, { 0.f, 0.f, -1.f }, {1.f, 0.f}}, //1
        { { +0.5f, -0.5f, -0.5f }, { 0.f, 0.f, -1.f }, {1.f, 1.f}}, //2
        { { -0.5f, -0.5f, -0.5f }, { 0.f, 0.f, -1.f }, {0.f, 1.f}}, //3
        { { -0.5f, +0.5f,  0.5f }, { 0.f, 0.f, +1.f }, {1.f, 0.f} }, //4 - back
        { { +0.5f, +0.5f,  0.5f }, { 0.f, 0.f, +1.f }, {0.f, 0.f} }, //5
        { { +0.5f, -0.5f,  0.5f }, { 0.f, 0.f, +1.f }, {0.f, 1.f} }, //6
        { { -0.5f, -0.5f,  0.5f }, { 0.f, 0.f, +1.f }, {1.f, 1.f} }, //7

        { { -0.5f, +0.5f,  0.5f }, { 0.f, +1.f, 0.f }, {0.f, 0.f} }, //8 - top
        { { +0.5f, +0.5f,  0.5f }, { 0.f, +1.f, 0.f }, {1.f, 0.f} }, //9
        { { +0.5f, +0.5f, -0.5f }, { 0.f, +1.f, 0.f }, {1.f, 1.f} }, //10
        { { -0.5f, +0.5f, -0.5f }, { 0.f, +1.f, 0.f }, {0.f, 1.f} }, //11
        { { -0.5f, -0.5f,  0.5f }, { 0.f, -1.f, 0.f }, {1.f, 0.f} }, //12 - bottom
        { { +0.5f, -0.5f,  0.5f }, { 0.f, -1.f, 0.f }, {0.f, 0.f} }, //13
        { { +0.5f, -0.5f, -0.5f }, { 0.f, -1.f, 0.f }, {0.f, 1.f} }, //14
        { { -0.5f, -0.5f, -0.5f }, { 0.f, -1.f, 0.f }, {1.f, 1.f} }, //15

        { { +0.5f, +0.5f, -0.5f }, { +1.f, 0.f, 0.f }, {0.f, 0.f} }, //16 - right
        { { +0.5f, +0.5f, +0.5f }, { +1.f, 0.f, 0.f }, {1.f, 0.f} }, //17
        { { +0.5f, -0.5f, +0.5f }, { +1.f, 0.f, 0.f }, {1.f, 1.f} }, //18
        { { +0.5f, -0.5f, -0.5f }, { +1.f, 0.f, 0.f }, {0.f, 1.f} }, //19
        { { -0.5f, +0.5f, -0.5f }, { -1.f, 0.f, 0.f }, {1.f, 0.f} }, //20 - left
        { { -0.5f, +0.5f, +0.5f }, { -1.f, 0.f, 0.f }, {0.f, 0.f} }, //21
        { { -0.5f, -0.5f, +0.5f }, { -1.f, 0.f, 0.f }, {0.f, 1.f} }, //22
        { { -0.5f, -0.5f, -0.5f }, { -1.f, 0.f, 0.f }, {1.f, 1.f} }, //23       
    };

    const uint32_t kIndices[] {
        0,1,2,      0,2,3,      //front  -Red
        4,6,5,      4,7,6,      //back   -Red+Green=Yellow
        8,9,10,     8,10,11,    //top    -Green
        12,14,13,   12,15,14,   //bottom -Green+Blue=Cyan
        16,17,18,   16,18,19,   //right  -Blue
        20,22,21,   20,23,22,   //left   -Red+Blue=Magenta
    };
}

bool TriangleRenderer::Create(GraphicsDevice& graphicsDevice)
{
    ID3D11Device* device = graphicsDevice.Device();

    Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
    if(!ShaderCompiler::CompileFromFile(L"Shaders/Triangle.hlsl", "VSMain", "vs_5_0", vertexShaderBlob))
        return false;

    Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
    if(!ShaderCompiler::CompileFromFile(L"Shaders/Triangle.hlsl", "PSMain", "ps_5_0", pixelShaderBlob))
        return false;

    HR_CHECK(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &mVertexShader));

    HR_CHECK(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &mPixelShader));

    const D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    HR_CHECK(device->CreateInputLayout(inputElements, ARRAYSIZE(inputElements), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &mInputLayout));


    if(!mVertexBuffer.Create(graphicsDevice, kVertices, ARRAYSIZE(kVertices))) 
        return false;

    if(!mIndexBuffer.Create(graphicsDevice, kIndices, ARRAYSIZE(kIndices)))
        return false;
    
    if(!mConstantBuffer.Create(graphicsDevice))
        return false;

    if(!mTexture.CreateFromFile(graphicsDevice, "../../../../../Assets/Textures/uv_checker_256.png"))
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

    device->CreateSamplerState(&desc, mSamplerState.GetAddressOf());

    Core::LogInfo("Triangle renderer created.");
    
    return true;
}

void TriangleRenderer::Render(ID3D11DeviceContext* context, const Matrix4x4& wvp)
{
    // const UINT stride = sizeof(Vertex);
    // const UINT offset = 0;
    // ID3D11Buffer* vertexBuffer = mVertexBuffer.Get();

    context->IASetInputLayout(mInputLayout.Get());
    mVertexBuffer.Bind(context);
    mIndexBuffer.Bind(context);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    ObjectConstant ojc{};
    ojc.wvp = wvp.Transposed();    
    
    if(!mConstantBuffer.Update(context, ojc))
        return;
    mConstantBuffer.BindVS(context, 0);

    context->VSSetShader(mVertexShader.Get(), nullptr, 0);

    mTexture.BindPS(context, 0);
    ID3D11SamplerState* sampler = mSamplerState.Get();
    context->PSSetSamplers(0, 1, &sampler);

    context->PSSetShader(mPixelShader.Get(), nullptr, 0);

    context->DrawIndexed(mIndexBuffer.Count(), 0, 0);
}
