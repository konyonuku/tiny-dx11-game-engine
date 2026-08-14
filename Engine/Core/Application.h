#pragma once
#include <cstdint>
#include <string>

#include "Core/GameTimer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Renderer.h"
#include "Graphics/SwapChain.h"
#include "Platform/Window.h"

class Application
{
public:
    struct Desc
    {
        const wchar_t* title  = L"Engine";
        uint32_t       width  = 1280;
        uint32_t       height = 720;
        bool           vsync  = true;
    };

    virtual ~Application() = default;

    int Run(const Desc& desc);

protected:
    virtual bool OnStart() { return true; }
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender(Renderer& renderer) {}

    GraphicsDevice&  Device()      { return mDevice; }
    Renderer&        GetRenderer() { return mRenderer; }
    Window&          GetWindow()   { return mWindow; }
    const GameTimer& Timer() const { return mTimer; }

private:
    bool Initialize(const Desc& desc);
    void MainLoop();
    void UpdateFrameStats();

    Window         mWindow;
    GraphicsDevice mDevice;
    SwapChain      mSwapChain;
    Renderer       mRenderer;
    GameTimer      mTimer;

    std::wstring mTitle;
    bool         mVsync        = true;
    uint32_t     mFrameCount   = 0;
    float        mStatsElapsed = 0.f;
};
