#include "Platform/Window.h"

#include "Core/Log.h"

namespace
{
    const wchar_t* kClassName = L"DX11Window";
}

LRESULT CALLBACK Window::WndProcThunk(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
    }

    auto* self = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    return self ? self->MsgProc(hWnd, msg, wp, lp)
                : DefWindowProcW(hWnd, msg, wp, lp);
}

LRESULT Window::MsgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_SIZE:
        mClientWidth  = LOWORD(lp);
        mClientHeight = HIWORD(lp);
        mMinimized    = (wp == SIZE_MINIMIZED);
        if (!mMinimized && !mResizing) NotifyResize();
        return 0;

    case WM_ENTERSIZEMOVE:
        mResizing = true;
        return 0;

    case WM_EXITSIZEMOVE:
        mResizing = false;
        NotifyResize();
        return 0;

    case WM_GETMINMAXINFO:
        reinterpret_cast<MINMAXINFO*>(lp)->ptMinTrackSize = { 200, 150 };
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wp, lp);
}

void Window::NotifyResize()
{
    if (OnResize && mClientWidth > 0 && mClientHeight > 0)
        OnResize(mClientWidth, mClientHeight);
}

bool Window::Create(HINSTANCE hInstance, const Desc& desc)
{
    WNDCLASSW wc = {};
    wc.lpfnWndProc   = WndProcThunk;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_ARROW));
    wc.lpszClassName = kClassName;

    if (!RegisterClassW(&wc)) {
        Core::LogError("RegisterClassW failed. err=%lu", GetLastError());
        return false;
    }

    RECT rc = { 0, 0, static_cast<LONG>(desc.width), static_cast<LONG>(desc.height) };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    mHwnd = CreateWindowExW(
        0, kClassName, desc.title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInstance, this);

    if (!mHwnd) {
        Core::LogError("CreateWindowExW failed. err=%lu", GetLastError());
        return false;
    }

    ShowWindow(mHwnd, SW_SHOW);

    RECT client {};
    GetClientRect(mHwnd, &client);
    mClientWidth  = client.right - client.left;
    mClientHeight = client.bottom - client.top;

    Core::LogInfo("Window created. client=%ux%u", mClientWidth, mClientHeight);
    return true;
}

bool Window::PumpMessages()
{
    MSG msg {};
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) return false;

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return true;
}

void Window::SetTitle(const wchar_t* title)
{
    SetWindowTextW(mHwnd, title);
}
