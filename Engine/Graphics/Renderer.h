#pragma once
#include <d3d11.h>

#include "Graphics/RenderState.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/Buffer.h"

class GraphicsDevice;
class SwapChain;


class Renderer
{
public:
    bool Create(GraphicsDevice& device, SwapChain& swapChain);
    bool DrawMesh(const Mesh& mesh, Material& material, const Matrix4x4& world);    
    bool SetFrameConstants(const FrameConstants& constants);
    
    void SetClearColor(float r, float g, float b, float a = 1.f);
    void BeginFrame();
    void EndFrame(bool vsync);

    ID3D11DeviceContext* Context() const;

private:
    GraphicsDevice* mDevice    = nullptr;
    SwapChain*      mSwapChain = nullptr;
    RenderState     mDefaultRenderState;
    ConstantBuffer<FrameConstants>      mFrameConstantBuffer;
    ConstantBuffer<ObjectConstants>     mObjectConstantBuffer;
    bool mFrameConstantsReady = false;

    float mClearColor[4] { 0.f, 0.f, 0.5f, 1.f };
};
