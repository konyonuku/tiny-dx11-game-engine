#pragma once
#include <cstdint>

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