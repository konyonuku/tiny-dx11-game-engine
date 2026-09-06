#pragma once

#include <cstdint>
#include <d3d11.h>

#include "Graphics/Buffer.h"


class Mesh
{
public:
    template <typename T>    
    bool Create(GraphicsDevice& device, const T* vertices, uint32_t vertexCount, const uint32_t* index, uint32_t indexCount);
    void Bind(ID3D11DeviceContext* context) const;
    void Draw(ID3D11DeviceContext *context) const;

private:
    VertexBuffer mVertexBuffer;
    IndexBuffer  mIndexBuffer;
};

template <typename T>
inline bool Mesh::Create(GraphicsDevice &device, const T *vertices, uint32_t vertexCount, const uint32_t *index, uint32_t indexCount)
{
    if(!mVertexBuffer.Create(device, vertices, vertexCount)) return false;
    if(!mIndexBuffer.Create(device, index, indexCount)) return false;
    return true;
}
