#pragma once
#include <memory>

#include "GameFramework/ActorComponent.h"

class Mesh;
class Material;


class StaticMeshComponent : public ActorComponent
{
public:
    void SetMesh(std::shared_ptr<Mesh> mesh);
    void SetMaterial(std::shared_ptr<Material> material);
    void SetVisible(bool visible);
    const std::shared_ptr<Mesh>& GetMesh() const;
    const std::shared_ptr<Material>& GetMaterial() const;
    bool IsVisible() const;

protected:
    void CollectRenderItems(std::vector<RenderItem>& items) const override; 

private:
    std::shared_ptr<Mesh>       mMesh;
    std::shared_ptr<Material>   mMaterial;
    bool mVisible = true;
};