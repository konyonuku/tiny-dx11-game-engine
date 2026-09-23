#include "DemoAssets.h"

#include "Core/Log.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Vertex.h"
#include "Resources/ResourceManager.h"


namespace
{
    const Vertex kVertices[] = {
        { { -0.5f, +0.5f, -0.5f }, { 0.f, 0.f, -1.f }, {0.f, 0.f}}, //0 - front
        { { +0.5f, +0.5f, -0.5f }, { 0.f, 0.f, -1.f }, {1.f, 0.f}}, //1
        { { +0.5f, -0.5f, -0.5f }, { 0.f, 0.f, -1.f }, {1.f, 1.f}}, //2
        { { -0.5f, -0.5f, -0.5f }, { 0.f, 0.f, -1.f }, {0.f, 1.f}}, //3
        { { -0.5f, +0.5f,  0.5f }, { 0.f, 0.f, +1.f }, {1.f, 0.f} }, //4 - back
        { { +0.5f, +0.5f,  0.5f }, { 0.f, 0.f, +1.f }, {0.f, 0.f} }, //5
        { { +0.5f, -0.5f,  0.5f }, { 0.f, 0.f, +1.f }, {0.f, 1.f} }, //6
        { { -0.5f, -0.5f,  0.5f }, { 0.f, 0.f, +1.f }, {1.f, 1.f} }, //7

        { { -0.5f, +0.5f,  0.5f }, { 0.f, +1.f, 0.f }, {0.f, 0.f} }, //8 - top
        { { +0.5f, +0.5f,  0.5f }, { 0.f, +1.f, 0.f }, {1.f, 0.f} }, //9
        { { +0.5f, +0.5f, -0.5f }, { 0.f, +1.f, 0.f }, {1.f, 1.f} }, //10
        { { -0.5f, +0.5f, -0.5f }, { 0.f, +1.f, 0.f }, {0.f, 1.f} }, //11
        { { -0.5f, -0.5f,  0.5f }, { 0.f, -1.f, 0.f }, {1.f, 0.f} }, //12 - bottom
        { { +0.5f, -0.5f,  0.5f }, { 0.f, -1.f, 0.f }, {0.f, 0.f} }, //13
        { { +0.5f, -0.5f, -0.5f }, { 0.f, -1.f, 0.f }, {0.f, 1.f} }, //14
        { { -0.5f, -0.5f, -0.5f }, { 0.f, -1.f, 0.f }, {1.f, 1.f} }, //15

        { { +0.5f, +0.5f, -0.5f }, { +1.f, 0.f, 0.f }, {0.f, 0.f} }, //16 - right
        { { +0.5f, +0.5f, +0.5f }, { +1.f, 0.f, 0.f }, {1.f, 0.f} }, //17
        { { +0.5f, -0.5f, +0.5f }, { +1.f, 0.f, 0.f }, {1.f, 1.f} }, //18
        { { +0.5f, -0.5f, -0.5f }, { +1.f, 0.f, 0.f }, {0.f, 1.f} }, //19
        { { -0.5f, +0.5f, -0.5f }, { -1.f, 0.f, 0.f }, {1.f, 0.f} }, //20 - left
        { { -0.5f, +0.5f, +0.5f }, { -1.f, 0.f, 0.f }, {0.f, 0.f} }, //21
        { { -0.5f, -0.5f, +0.5f }, { -1.f, 0.f, 0.f }, {0.f, 1.f} }, //22
        { { -0.5f, -0.5f, -0.5f }, { -1.f, 0.f, 0.f }, {1.f, 1.f} }, //23       
    };

    const uint32_t kIndices[] {
        0,1,2,      0,2,3,      //front  -Red
        4,6,5,      4,7,6,      //back   -Red+Green=Yellow
        8,9,10,     8,10,11,    //top    -Green
        12,14,13,   12,15,14,   //bottom -Green+Blue=Cyan
        16,17,18,   16,18,19,   //right  -Blue
        20,22,21,   20,23,22,   //left   -Red+Blue=Magenta
    };
}

bool DemoAssets::Create(GraphicsDevice& graphicsDevice, ResourceManager& resources)
{
    constexpr const char* kShaderKey = "Shaders/Default.hlsl";
    constexpr const char* kTextureKey = "Textures/uv_checker_256.png";

    std::shared_ptr<Shader> newShader = resources.LoadShaderPNUV(kShaderKey);
    if(!newShader) return false;

    std::shared_ptr<Texture2D> newTexture = resources.LoadTexture(kTextureKey);
    if(!newTexture) return false;

    // Procedural mesh: no file to load, so create once and register under a reserved key.
    std::shared_ptr<Mesh> newMesh = resources.FindMesh(kCubeMeshKey);
    if(!newMesh) {
        newMesh = std::make_shared<Mesh>();
        if(!newMesh->Create(graphicsDevice, kVertices, static_cast<uint32_t>(std::size(kVertices)), kIndices, static_cast<uint32_t>(std::size(kIndices))))
            return false;
        if(!resources.RegisterMesh(kCubeMeshKey, newMesh))
            return false;
    }

    auto newMaterial = std::make_shared<Material>();
    if(!newMaterial->Create(graphicsDevice, newShader, newTexture)) return false;

    shader = std::move(newShader);
    diffuseTexture = std::move(newTexture);
    cubeMesh = std::move(newMesh);
    material = std::move(newMaterial);
    Core::LogInfo("Shared demo assets created.");

    return true;
}
