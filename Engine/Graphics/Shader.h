#pragma once
#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>

class GraphicsDevice;

using Microsoft::WRL::ComPtr;


class Shader {
public:
    bool Create(GraphicsDevice &graphicsDevice, const wchar_t *fileName, const D3D11_INPUT_ELEMENT_DESC *inputElements, uint32_t inputElementCount);
    void Bind(ID3D11DeviceContext* context) const;
private:
    ComPtr<ID3D11VertexShader> mVertexShader;
    ComPtr<ID3D11PixelShader>  mPixelShader;
    ComPtr<ID3D11InputLayout>  mInputLayout;
};