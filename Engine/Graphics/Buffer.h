#pragma once
#include <cstdint>
#include <type_traits>

#include "Graphics/GraphicsDevice.h"

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

