#pragma once
#include <memory>

class GraphicsDevice;
class Mesh;
class Shader;
class Texture2D;
class Material;
class ResourceManager;


struct DemoAssets
{
    // Procedural cube is registered under this key; file meshes are loaded by their path key.
    static constexpr const char* kCubeMeshKey = "Procedural/UnitCube";

    bool Create(GraphicsDevice& device, ResourceManager& resources);

    std::shared_ptr<Mesh> cubeMesh;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture2D> diffuseTexture;
    std::shared_ptr<Material> material;
};
