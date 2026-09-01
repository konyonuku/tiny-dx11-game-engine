#pragma once

#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class GraphicsDevice;

class Texture2D
{
public:
    bool CreateFromFile(GraphicsDevice& device, const char* filename);
    void BindPS(ID3D11DeviceContext* context, uint32_t slot = 0) const;

    uint32_t Width() const { return mWidth; }
    uint32_t Height() const { return mHeight; }

private:
    ComPtr<ID3D11Texture2D> mTexture;
    ComPtr<ID3D11ShaderResourceView> mSRV;

    uint32_t mWidth = 0;
    uint32_t mHeight = 0;
};