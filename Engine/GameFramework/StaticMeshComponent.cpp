#include "StaticMeshComponent.h"

#include "GameFramework/Actor.h"


void StaticMeshComponent::SetMesh(std::shared_ptr<Mesh> mesh)
{
    mMesh = mesh;
}

void StaticMeshComponent::SetMaterial(std::shared_ptr<Material> material)
{
    mMaterial = material;
}

void StaticMeshComponent::SetVisible(bool visible)
{
    mVisible = visible;
}

void StaticMeshComponent::CollectRenderItems(std::vector<RenderItem> &items) const
{
    if(!mVisible || !mMesh || !mMaterial || !GetOwner()) return;
    RenderItem item {};
    item.mesh = mMesh;
    item.material = mMaterial;
    item.world = GetOwner()->GetTransform().WorldMatrix();
    items.push_back(item);
}

const std::shared_ptr<Mesh> &StaticMeshComponent::GetMesh() const
{
    return mMesh;
}

const std::shared_ptr<Material> &StaticMeshComponent::GetMaterial() const
{
    return mMaterial;
}

bool StaticMeshComponent::IsVisible() const
{
    return mVisible;
}
