#include "Material.h"


bool Material::Create(GraphicsDevice &device, const char *diffuseTextPath)
{
    if(!mDiffuseTexture.CreateFromFile(device, diffuseTextPath))
        return false;

    if(!mConstantBuffer.Create(device))
        return false;

    return true;
}

bool Material::BindPS(ID3D11DeviceContext *context)
{
    if(!mConstantBuffer.Update(context, mConstants))
        return false;

    mConstantBuffer.BindPS(context, 2);
    mDiffuseTexture.BindPS(context, 0);

    return true;
}
