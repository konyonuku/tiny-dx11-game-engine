#include "Core/Application.h"

#include <sstream>

#include "Core/Log.h"

int Application::Run(const Desc& desc)
{
    if (!Initialize(desc)) return 1;
    if (!OnStart()) return 1;

    mTimer.Reset();
    MainLoop();

    Core::LogInfo("Exiting.");
    return 0;
}

bool Application::Initialize(const Desc& desc)
{
    mTitle = desc.title;
    mVsync = desc.vsync;

    Window::Desc windowDesc;
    windowDesc.title  = desc.title;
    windowDesc.width  = desc.width;
    windowDesc.height = desc.height;

    if (!mWindow.Create(GetModuleHandleW(nullptr), windowDesc)) return false;
    if (!mDevice.Create()) return false;
    if (!mSwapChain.Create(mDevice, mWindow.Handle(),
                           mWindow.ClientWidth(), mWindow.ClientHeight()))
        return false;
    if (!mRenderer.Create(mDevice, mSwapChain)) return false;

    mWindow.OnResize = [this](uint32_t w, uint32_t h) { mSwapChain.Resize(w, h); };
    return true;
}

void Application::MainLoop()
{
    while (mWindow.PumpMessages()) {
        mTimer.Tick();
        UpdateFrameStats();

        if (mWindow.IsMinimized()) {
            Sleep(16);
            continue;
        }

        OnUpdate(mTimer.DeltaTime());

        mRenderer.BeginFrame();
        OnRender(mRenderer);
        mRenderer.EndFrame(mVsync);
    }
}

void Application::UpdateFrameStats()
{
    ++mFrameCount;

    if (mTimer.TotalTime() - mStatsElapsed < 1.0f) return;

    const int   fps  = static_cast<int>(mFrameCount);
    const float mspf = 1000.0f / static_cast<float>(mFrameCount);

    std::wostringstream outs;
    outs.precision(6);
    outs << mTitle
         << L"   Fps: " << fps
         << L"   Frame Time: " << mspf << L" (ms)";

    mWindow.SetTitle(outs.str().c_str());

    mFrameCount = 0;
    mStatsElapsed += 1.0f;
}
