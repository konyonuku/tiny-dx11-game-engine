#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Library/stb_image.h"
#include "Graphics/GraphicsDevice.h"
#include "Core/Log.h"

bool Texture2D::CreateFromFile(GraphicsDevice &device, const char *filename)
{
    int width = 0, height = 0, channels = 0;
    unsigned char *pData = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

    if(pData == nullptr) {
        Core::LogError("failed to load Texture2D Image: %s", filename);
        return false;
    }

    Core::LogInfo("Texture2d Image Loaded(width=%d, height=%d)", width, height);
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width      = width;
    desc.Height     = height;
    desc.MipLevels  = 1;
    desc.ArraySize  = 1;
    desc.Format     = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Usage      = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags  = D3D11_BIND_SHADER_RESOURCE;
    desc.SampleDesc.Count = 1;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem          = pData;
    initData.SysMemPitch      = width * 4;
    initData.SysMemSlicePitch = 0;
    
    
    // HR_CHECK(device.Device()->CreateTexture2D(&desc, &initData, mTexture.GetAddressOf()));
    HRESULT hr = device.Device()->CreateTexture2D(&desc, &initData, mTexture.GetAddressOf());
    stbi_image_free(pData);
    pData = nullptr;

    if(FAILED(hr)) {
        Core::LogHrFailure("ID3D11Device::CreateTexture2D", hr, __FILE__, __LINE__);
        return false;
    }
    Core::LogInfo("Texture2D created.");

    HR_CHECK(device.Device()->CreateShaderResourceView(mTexture.Get(), nullptr, mSRV.GetAddressOf()));
    Core::LogInfo("ShaderResourceView created.");
    
    mWidth = static_cast<uint32_t>(width);
    mHeight = static_cast<uint32_t>(height);

    
    return true;
}

void Texture2D::BindPS(ID3D11DeviceContext *context, uint32_t slot) const
{
    ID3D11ShaderResourceView* srv = mSRV.Get();
    context->PSSetShaderResources(slot, 1, &srv);
}
