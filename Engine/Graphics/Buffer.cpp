#include "Graphics/Buffer.h"

#include "Core/Log.h"
#include "Buffer.h"

void VertexBuffer::Bind(ID3D11DeviceContext *context, uint32_t slot, uint32_t offset) const
{
    const UINT stride = mStride;
    const UINT off = offset;
    
    context->IASetVertexBuffers(slot, 1, mBuffer.GetAddressOf(), &stride, &off);
}

bool VertexBuffer::CreateInternal(GraphicsDevice &device, const void *data, uint32_t stride, uint32_t count)
{
    if (data == nullptr || stride == 0 || count == 0) {
        Core::LogError("VertexBuffer::Create invalid args. stride=%u count=%u", stride, count);
        return false;
    }

    D3D11_BUFFER_DESC bufferDesc {};
    bufferDesc.ByteWidth = stride * count;
    bufferDesc.Usage     = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initialData {};
    initialData.pSysMem = data;

    HR_CHECK(device.Device()->CreateBuffer(&bufferDesc, &initialData, &mBuffer));

    mStride = stride;
    mCount  = count;

    Core::LogInfo("VertexBuffer created. stride=%u count=%u", stride, count);
    return true;
}
