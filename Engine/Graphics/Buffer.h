#pragma once
#include <cstring>
#include <cstdint>
#include <type_traits>

#include "Graphics/GraphicsDevice.h"
#include "Core/Log.h"


class VertexBuffer
{
public:
    template <typename T>
    bool Create(GraphicsDevice& device, const T* vertices, uint32_t count) {
        return CreateInternal(device, vertices, sizeof(T), count);
    }
    
    uint32_t Stride() const { return mStride; }
    uint32_t Count()  const { return mCount; }
    void Bind(ID3D11DeviceContext* context, uint32_t slot = 0, uint32_t offset = 0) const;

private:
    bool CreateInternal(GraphicsDevice& device, const void* data, uint32_t stride, uint32_t count);

    ComPtr<ID3D11Buffer> mBuffer;
    uint32_t mStride = 0;
    uint32_t mCount  = 0;
};



class IndexBuffer
{
public:
    //fixed index type: uint32_t(->DXGI_FORMAT_R32_UINT)
    bool Create(GraphicsDevice& device, const uint32_t* index, uint32_t count);
    
    uint32_t Count() const { return mCount; }
    void Bind(ID3D11DeviceContext* context, uint32_t offset = 0) const;

private:
    ComPtr<ID3D11Buffer> mBuffer;
    uint32_t mCount = 0;
};


template <typename T>
class ConstantBuffer
{
public:
    bool Create(GraphicsDevice& device) {
        static_assert(sizeof(T) % 16 == 0, "ConstantBuffer data size is not a multiple of 16");

        D3D11_BUFFER_DESC desc{};
        desc.ByteWidth      = sizeof(T);
        desc.Usage          = D3D11_USAGE_DYNAMIC;
        desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HR_CHECK(device.Device()->CreateBuffer(&desc, nullptr, &mBuffer));
        Core::LogInfo("ConstantBuffer created. size=%u", sizeof(T));

        return true;
    }

    bool Update(ID3D11DeviceContext* context, const T& data) {
        // typedef struct D3D11_MAPPED_SUBRESOURCE {
        //   void *pData;
        //   UINT RowPitch;
        //   UINT DepthPitch;
        // } D3D11_MAPPED_SUBRESOURCE;
        D3D11_MAPPED_SUBRESOURCE mapResource{};

        HR_CHECK(context->Map(mBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource));
        std::memcpy(mapResource.pData, &data, sizeof(T));
        context->Unmap(mBuffer.Get(), 0);


        return true;
    }

    void BindVS(ID3D11DeviceContext* context, uint32_t slot = 0) const {
        context->VSSetConstantBuffers(slot, 1, mBuffer.GetAddressOf());
    }

    // void BindPS(ID3D11DeviceContext* context, uint32_t slot = 0) const;

private:
    ComPtr<ID3D11Buffer> mBuffer;
};
