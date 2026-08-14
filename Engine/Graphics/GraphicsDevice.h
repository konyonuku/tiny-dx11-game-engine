#pragma once
#include <d3d11.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class GraphicsDevice
{
public:
    bool Create();

    ID3D11Device*        Device()  const { return mDevice.Get(); }
    ID3D11DeviceContext* Context() const { return mContext.Get(); }
    IDXGIFactory2*       Factory() const { return mFactory.Get(); }

    bool AllowTearing() const { return mAllowTearing; }

private:
    bool CreateFactory();
    bool SelectAdapter(ComPtr<IDXGIAdapter1>& outAdapter);
    bool CreateDevice(IDXGIAdapter1* adapter);
    void SetupDebugLayer();
    void QueryTearingSupport();

    ComPtr<IDXGIFactory2>       mFactory;
    ComPtr<ID3D11Device>        mDevice;
    ComPtr<ID3D11DeviceContext> mContext;

    D3D_FEATURE_LEVEL mFeatureLevel  = D3D_FEATURE_LEVEL_11_0;
    bool              mAllowTearing  = false;
};
