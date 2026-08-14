#include "Graphics/GraphicsDevice.h"

#include <d3d11sdklayers.h>

#include "Core/Log.h"

bool GraphicsDevice::Create()
{
    if (!CreateFactory()) return false;

    ComPtr<IDXGIAdapter1> adapter;
    if (!SelectAdapter(adapter)) return false;
    if (!CreateDevice(adapter.Get())) return false;

    SetupDebugLayer();
    QueryTearingSupport();
    return true;
}

bool GraphicsDevice::CreateFactory()
{
    UINT flags = 0;
#if defined(_DEBUG)
    flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    if (FAILED(CreateDXGIFactory2(flags, IID_PPV_ARGS(&mFactory))))
        HR_CHECK(CreateDXGIFactory2(0, IID_PPV_ARGS(&mFactory)));

    return true;
}

bool GraphicsDevice::SelectAdapter(ComPtr<IDXGIAdapter1>& outAdapter)
{
    ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(mFactory.As(&factory6))) {
        if (SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&outAdapter)))) {
            DXGI_ADAPTER_DESC1 desc {};
            outAdapter->GetDesc1(&desc);
            Core::LogInfo("Adapter: %S (VRAM %llu MB)", desc.Description,
                          static_cast<unsigned long long>(desc.DedicatedVideoMemory >> 20));
            return true;
        }
    }

    HR_CHECK(mFactory->EnumAdapters1(0, &outAdapter));

    DXGI_ADAPTER_DESC1 desc {};
    outAdapter->GetDesc1(&desc);
    Core::LogInfo("Adapter (fallback): %S (VRAM %llu MB)", desc.Description,
                  static_cast<unsigned long long>(desc.DedicatedVideoMemory >> 20));
    return true;
}

bool GraphicsDevice::CreateDevice(IDXGIAdapter1* adapter)
{
    const D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    UINT flags = 0;
#if defined(_DEBUG)
    flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags,
                                   levels, ARRAYSIZE(levels), D3D11_SDK_VERSION,
                                   &mDevice, &mFeatureLevel, &mContext);

    if (FAILED(hr) && flags != 0) {
        hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0,
                               levels, ARRAYSIZE(levels), D3D11_SDK_VERSION,
                               &mDevice, &mFeatureLevel, &mContext);
    }
    HR_CHECK(hr);

    if (mFeatureLevel < D3D_FEATURE_LEVEL_11_0) {
        Core::LogError("Feature Level 11.0 unsupported. level=0x%04X", mFeatureLevel);
        return false;
    }

    Core::LogInfo("Device created. featureLevel=0x%04X", mFeatureLevel);
    return true;
}

void GraphicsDevice::SetupDebugLayer()
{
#if defined(_DEBUG)
    ComPtr<ID3D11InfoQueue> infoQueue;
    if (SUCCEEDED(mDevice.As(&infoQueue))) {
        infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
    }
#endif
}

void GraphicsDevice::QueryTearingSupport()
{
    ComPtr<IDXGIFactory5> factory5;
    BOOL allow = FALSE;

    if (SUCCEEDED(mFactory.As(&factory5)) &&
        SUCCEEDED(factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                                                &allow, sizeof(allow)))) {
        mAllowTearing = (allow == TRUE);
    }

    Core::LogInfo("Tearing support: %s", mAllowTearing ? "yes" : "no");
}
