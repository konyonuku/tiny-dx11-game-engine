#include "Graphics/ShaderCompiler.h"

#include <d3dcompiler.h>

#include "Core/Log.h"

namespace
{
    std::filesystem::path ExecutableDir()
    {
        wchar_t modulePath[MAX_PATH] {};
        const DWORD length = GetModuleFileNameW(nullptr, modulePath, ARRAYSIZE(modulePath));
        if (length == 0 || length == ARRAYSIZE(modulePath)) return {};

        return std::filesystem::path(modulePath).parent_path();
    }
}

namespace ShaderCompiler
{
    bool CompileFromFile(const std::filesystem::path& path, const char* entryPoint,
                         const char* target, Microsoft::WRL::ComPtr<ID3DBlob>& outBlob)
    {
        std::filesystem::path fullPath = path;
        if (fullPath.is_relative()) {
            const std::filesystem::path baseDir = ExecutableDir();
            if (baseDir.empty()) {
                Core::LogError("Failed to resolve the executable path. err=%lu", GetLastError());
                return false;
            }
            fullPath = baseDir / fullPath;
        }

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
        const HRESULT hr = D3DCompileFromFile(
            fullPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entryPoint, target, flags, 0, &outBlob, &errorBlob);

        if (FAILED(hr)) {
            if (errorBlob) {
                Core::LogError("Shader compilation failed (%s, %s):\n%s", entryPoint, target,
                               static_cast<const char*>(errorBlob->GetBufferPointer()));
            } else {
                Core::LogHrFailure("D3DCompileFromFile", hr, __FILE__, __LINE__);
            }
            return false;
        }

        return true;
    }
}
