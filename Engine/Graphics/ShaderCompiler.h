#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include <filesystem>

namespace ShaderCompiler
{
    bool CompileFromFile(const std::filesystem::path& path, const char* entryPoint,
                         const char* target, Microsoft::WRL::ComPtr<ID3DBlob>& outBlob);
}
