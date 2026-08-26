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
        float color[4];
    };

    const Vertex kVertices[] = {
        { { -0.5f,  0.5f, 0.0f }, { 1.f, 0.f, 0.f, 1.f } },
        { {  0.5f,  0.5f, 0.0f }, { 0.f, 1.f, 0.f, 1.f } },
        { {  0.5f, -0.5f, 0.0f }, { 0.f, 0.f, 1.f, 1.f } },
        { { -0.5f, -0.5f, 0.0f }, { 1.f, 1.f, 0.f, 1.f } },
    };

    const uint32_t kIndices[] {
        0, 1, 2,
        0, 2, 3,
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
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    HR_CHECK(device->CreateInputLayout(inputElements, ARRAYSIZE(inputElements), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &mInputLayout));


    if(!mVertexBuffer.Create(graphicsDevice, kVertices, ARRAYSIZE(kVertices))) 
        return false;

    if(!mIndexBuffer.Create(graphicsDevice, kIndices, ARRAYSIZE(kIndices)))
        return false;
    
    Core::LogInfo("Triangle renderer created.");
    return true;
}

void TriangleRenderer::Render(ID3D11DeviceContext* context)
{
    // const UINT stride = sizeof(Vertex);
    // const UINT offset = 0;
    // ID3D11Buffer* vertexBuffer = mVertexBuffer.Get();

    context->IASetInputLayout(mInputLayout.Get());
    mVertexBuffer.Bind(context);
    mIndexBuffer.Bind(context);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->VSSetShader(mVertexShader.Get(), nullptr, 0);
    context->PSSetShader(mPixelShader.Get(), nullptr, 0);

    context->DrawIndexed(mIndexBuffer.Count(), 0, 0);
}
