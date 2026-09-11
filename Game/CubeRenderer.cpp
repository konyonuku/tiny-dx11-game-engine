#include "CubeRenderer.h"

#include <cstddef>

#include "Core/Log.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ShaderCompiler.h"

namespace
{
    struct Vertex
    {
        float position[3];
        float normal[3];
        float uv[2];
    };

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

bool CubeRenderer::Create(GraphicsDevice& graphicsDevice)
{
    ID3D11Device* device = graphicsDevice.Device();

    const D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    if(!mShader.Create(graphicsDevice, L"Shaders/Default.hlsl", inputElements, ARRAYSIZE(inputElements)))
        return false;

    if(!mMesh.Create(graphicsDevice, kVertices, ARRAYSIZE(kVertices), kIndices, ARRAYSIZE(kIndices))) 
        return false;
    
    if(!mObjectConstantBuffer.Create(graphicsDevice))
        return false;

    if(!mLightConstantBuffer.Create(graphicsDevice))
        return false;

    if(!mMaterial.Create(graphicsDevice, mShader, "../../../../../Assets/Textures/uv_checker_256.png"))
        return false;

    Core::LogInfo("Default renderer created.");
    
    return true;
}

void CubeRenderer::Render(ID3D11DeviceContext* context, const Matrix4x4& world, const Matrix4x4& wvp, const Vector3& camera)
{
    mMesh.Bind(context);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ObjectConstant ojc{};
    ojc.wvp = wvp.Transposed();
    ojc.world = world.Transposed();
    if(!mObjectConstantBuffer.Update(context, ojc)) return;
    mObjectConstantBuffer.BindVS(context, 0); // connecting to b0 in VS

    LightConstant light {};
    light.lightDirection    = { 1.0f, -1.0f, 1.0f}; // direction from light source to surface 
    light.lightColor        = { 1.0f,  1.0f,  1.0f};
    light.lightIntensity    = 1.0f;
    light.ambientIntensity  = 0.05f;
    light.cameraPosition    = camera;
    light.padding           = 0.0f;
    if(!mLightConstantBuffer.Update(context, light)) return;
    mLightConstantBuffer.BindPS(context, 1); // connecting to b1 in PS
    
    // connecting <MaterialConstant> to b2 in PS
    // connecting <DiffuseTexture> to t0 in PS 
    // connecting <LinearSampler> to s0 in PS 
    if(!mMaterial.Bind(context)) return;  

    mMesh.Draw(context);
}
