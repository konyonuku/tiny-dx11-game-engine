#include "Resources/ResourceManager.h"

#include <cstddef>
#include <iterator>
#include <system_error>
#include <utility>

#include "Core/Log.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Vertex.h"


bool ResourceManager::Initialize(GraphicsDevice& device, const std::filesystem::path& absoluteAssetRoot)
{
    if(mDevice) {
        Core::LogError("ResourceManager is already initialized.");
        return false;
    }
    if(absoluteAssetRoot.empty() || !absoluteAssetRoot.is_absolute()) {
        Core::LogError("Asset root must be an absolute path: %s", absoluteAssetRoot.string().c_str());
        return false;
    }

    std::error_code error;
    if(!std::filesystem::is_directory(absoluteAssetRoot, error)) {
        Core::LogError("Asset root is not a directory: %s", absoluteAssetRoot.string().c_str());
        return false;
    }

    mAssetRoot = absoluteAssetRoot.lexically_normal();
    mDevice = &device;
    Core::LogInfo("Asset root: %s", mAssetRoot.string().c_str());
    return true;
}

std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const std::string& key)
{
    if(!mDevice) return nullptr;

    const std::optional<std::string> normalized = NormalizeKey(key);
    if(!normalized) {
        Core::LogError("Invalid texture key: '%s'", key.c_str());
        return nullptr;
    }

    if(auto found = mTextures.find(*normalized); found != mTextures.end())
        return found->second;

    const std::string fullPath = (mAssetRoot / *normalized).string();
    auto texture = std::make_shared<Texture2D>();
    if(!texture->CreateFromFile(*mDevice, fullPath.c_str())) {
        Core::LogError("Texture load failed. key=%s path=%s", normalized->c_str(), fullPath.c_str());
        return nullptr;
    }

    mTextures.emplace(*normalized, texture);
    return texture;
}

std::shared_ptr<Shader> ResourceManager::LoadShaderPNUV(const std::string& key)
{
    if(!mDevice) return nullptr;

    const std::optional<std::string> normalized = NormalizeKey(key);
    if(!normalized) {
        Core::LogError("Invalid shader key: '%s'", key.c_str());
        return nullptr;
    }

    if(auto found = mShaders.find(*normalized); found != mShaders.end())
        return found->second;

    const D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal),   D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(Vertex, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    const std::filesystem::path fullPath = mAssetRoot / *normalized;
    const std::wstring widePath = fullPath.wstring();
    auto shader = std::make_shared<Shader>();
    if(!shader->Create(*mDevice, widePath.c_str(), inputElements, static_cast<uint32_t>(std::size(inputElements)))) {
        Core::LogError("Shader load failed. key=%s path=%s", normalized->c_str(), fullPath.string().c_str());
        return nullptr;
    }

    mShaders.emplace(*normalized, shader);
    return shader;
}

std::shared_ptr<Mesh> ResourceManager::FindMesh(const std::string& key) const
{
    if(!mDevice) return nullptr;

    const std::optional<std::string> normalized = NormalizeKey(key);
    if(!normalized) return nullptr;

    const auto found = mMeshes.find(*normalized);
    return found != mMeshes.end() ? found->second : nullptr;
}

bool ResourceManager::RegisterMesh(const std::string& key, std::shared_ptr<Mesh> mesh)
{
    if(!mDevice || !mesh) return false;

    const std::optional<std::string> normalized = NormalizeKey(key);
    if(!normalized) {
        Core::LogError("Invalid mesh key: '%s'", key.c_str());
        return false;
    }

    const bool inserted = mMeshes.emplace(*normalized, std::move(mesh)).second;
    if(!inserted)
        Core::LogError("Mesh key already registered: %s", normalized->c_str());
    return inserted;
}

void ResourceManager::Clear()
{
    mMeshes.clear();
    mTextures.clear();
    mShaders.clear();
}

std::optional<std::string> ResourceManager::NormalizeKey(const std::string& key)
{
    if(key.empty()) return std::nullopt;

    const std::filesystem::path path(key);
    if(path.has_root_path()) return std::nullopt;

    const std::filesystem::path normal = path.lexically_normal();
    if(normal.empty() || normal == "." || !normal.has_filename()) return std::nullopt;
    if(*normal.begin() == "..") return std::nullopt;

    return normal.generic_string();
}
