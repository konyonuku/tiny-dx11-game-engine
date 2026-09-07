#include "Shader.h"

#include "Core/Log.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ShaderCompiler.h"


bool Shader::Create(GraphicsDevice &graphicsDevice, const wchar_t *fileName, const D3D11_INPUT_ELEMENT_DESC *inputElements, uint32_t inputElementCount)
{
    ID3D11Device* device = graphicsDevice.Device();

    ComPtr<ID3DBlob> vertexShaderBlob;
    if(!ShaderCompiler::CompileFromFile(fileName, "VSMain", "vs_5_0", vertexShaderBlob))
        return false;

    ComPtr<ID3DBlob> pixelShaderBlob;
    if(!ShaderCompiler::CompileFromFile(fileName, "PSMain", "ps_5_0", pixelShaderBlob))
        return false;

    HR_CHECK(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &mVertexShader));
    HR_CHECK(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &mPixelShader));
    HR_CHECK(device->CreateInputLayout(inputElements, inputElementCount, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &mInputLayout));

    
    return true;
}

void Shader::Bind(ID3D11DeviceContext *context) const
{
    context->IASetInputLayout(mInputLayout.Get());
    context->VSSetShader(mVertexShader.Get(), nullptr, 0);
    context->PSSetShader(mPixelShader.Get(), nullptr, 0);
}
