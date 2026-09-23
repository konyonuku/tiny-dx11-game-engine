#pragma once
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

class GraphicsDevice;
class Mesh;
class Texture2D;
class Shader;

// 경로로 캐싱
class ResourceManager
{
public:
    bool Initialize(GraphicsDevice& device,
                    const std::filesystem::path& absoluteAssetRoot);

    std::shared_ptr<Texture2D> LoadTexture(const std::string& key);
    std::shared_ptr<Shader> LoadShaderPNUV(const std::string& key);

    std::shared_ptr<Mesh> FindMesh(const std::string& key) const;
    bool RegisterMesh(const std::string& key, std::shared_ptr<Mesh> mesh);

    void Clear();

    bool IsInitialized() const { return mDevice != nullptr; }
    const std::filesystem::path& AssetRoot() const { return mAssetRoot; }

private:
    static std::optional<std::string> NormalizeKey(const std::string& key);

    GraphicsDevice* mDevice = nullptr;
    std::filesystem::path mAssetRoot;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> mMeshes;
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> mTextures;
    std::unordered_map<std::string, std::shared_ptr<Shader>> mShaders;
};
