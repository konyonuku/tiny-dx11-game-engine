#include "Mesh.h"

void Mesh::Bind(ID3D11DeviceContext *context) const
{
    mVertexBuffer.Bind(context);
    mIndexBuffer.Bind(context);
}

void Mesh::Draw(ID3D11DeviceContext *context) const
{
    context->DrawIndexed(mIndexBuffer.Count(), 0, 0);
}
