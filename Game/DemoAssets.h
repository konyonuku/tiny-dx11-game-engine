#pragma once
#include <memory>

class GraphicsDevice;
class Mesh;
class Shader;
class Material;


struct DemoAssets
{
    bool Create(GraphicsDevice& device);

    std::shared_ptr<Mesh> cubeMesh;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Material> material;
};
