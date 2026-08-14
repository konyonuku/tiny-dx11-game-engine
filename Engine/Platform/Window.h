#pragma once
#include <windows.h>

#include <cstdint>
#include <functional>

class Window
{
public:
    struct Desc
    {
        const wchar_t* title  = L"Engine";
        uint32_t       width  = 800;
        uint32_t       height = 600;
    };

    bool Create(HINSTANCE hInstance, const Desc& desc);

    bool PumpMessages();
    void SetTitle(const wchar_t* title);

    HWND     Handle()       const { return mHwnd; }
    uint32_t ClientWidth()  const { return mClientWidth; }
    uint32_t ClientHeight() const { return mClientHeight; }
    bool     IsMinimized()  const { return mMinimized; }

    std::function<void(uint32_t, uint32_t)> OnResize;

private:
    static LRESULT CALLBACK WndProcThunk(HWND, UINT, WPARAM, LPARAM);
    LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);
    void NotifyResize();

    HWND     mHwnd         = nullptr;
    uint32_t mClientWidth  = 0;
    uint32_t mClientHeight = 0;
    bool     mMinimized    = false;
    bool     mResizing     = false;
};
