#include "Mesh.h"

bool Mesh::Create(GraphicsDevice& device, const MeshData& data)
{
    if(data.vertices.empty() || data.indices.empty()) return false;
    return Create(device, data.vertices.data(), static_cast<uint32_t>(data.vertices.size()),
                  data.indices.data(), static_cast<uint32_t>(data.indices.size()));
}

void Mesh::Bind(ID3D11DeviceContext *context) const
{
    mVertexBuffer.Bind(context);
    mIndexBuffer.Bind(context);
}

void Mesh::Draw(ID3D11DeviceContext *context) const
{
    context->DrawIndexed(mIndexBuffer.Count(), 0, 0);
}
