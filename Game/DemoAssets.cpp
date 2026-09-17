#include "DemoAssets.h"

#include "Core/Log.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Vertex.h"


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

bool DemoAssets::Create(GraphicsDevice& graphicsDevice)
{
    const D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    auto newShader = std::make_shared<Shader>();
    auto newMesh = std::make_shared<Mesh>();
    auto newMaterial = std::make_shared<Material>();

    if(!newShader->Create(graphicsDevice, L"Shaders/Default.hlsl", inputElements, static_cast<uint32_t>(std::size(inputElements))))
        return false;

    if(!newMesh->Create(graphicsDevice, kVertices, static_cast<uint32_t>(std::size(kVertices)), kIndices, static_cast<uint32_t>(std::size(kIndices))))
        return false;

    if(!newMaterial->Create(graphicsDevice, newShader, "../../../../../Assets/Textures/uv_checker_256.png"))
        return false;

    shader = std::move(newShader);
    cubeMesh = std::move(newMesh);
    material = std::move(newMaterial);
    Core::LogInfo("Shared demo assets created.");

    return true;
}
