#pragma once
#include <d3d11.h>

class GraphicsDevice;
class SwapChain;

class Renderer
{
public:
    bool Create(GraphicsDevice& device, SwapChain& swapChain);

    void SetClearColor(float r, float g, float b, float a = 1.f);
    void BeginFrame();
    void EndFrame(bool vsync);

    ID3D11DeviceContext* Context() const;

private:
    GraphicsDevice* mDevice    = nullptr;
    SwapChain*      mSwapChain = nullptr;

    float mClearColor[4] { 0.f, 0.f, 0.5f, 1.f };
};
