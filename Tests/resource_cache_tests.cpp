#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "Graphics/GraphicsDevice.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"
#include "Resources/ResourceManager.h"

namespace fs = std::filesystem;

namespace
{
    // One real D3D11 device for the whole run. Resource creation needs the GPU;
    // no window or swap chain is required.
    GraphicsDevice gDevice;

    const fs::path kSourceAssets = fs::path(JITAI_SOURCE_ASSET_DIR);
    constexpr const char* kTextureKey = "Textures/uv_checker_256.png";
    constexpr const char* kShaderKey = "Shaders/Default.hlsl";

    void Expect(bool condition, const std::string& message)
    {
        if(!condition) throw std::runtime_error(message);
    }

    // Isolated asset root in %TEMP%, removed on scope exit.
    class TempAssetRoot
    {
    public:
        TempAssetRoot()
        {
            mRoot = fs::temp_directory_path() / "jitai_resource_cache_tests";
            std::error_code ignored;
            fs::remove_all(mRoot, ignored);
            fs::create_directories(mRoot / "Textures");
            fs::create_directories(mRoot / "Shaders");
        }
        ~TempAssetRoot()
        {
            std::error_code ignored;
            fs::remove_all(mRoot, ignored);
        }

        const fs::path& Path() const { return mRoot; }

        void CopyFromSource(const std::string& key) const
        {
            fs::copy_file(kSourceAssets / key, mRoot / key, fs::copy_options::overwrite_existing);
        }

        void WriteText(const std::string& key, const std::string& text) const
        {
            std::ofstream file(mRoot / key, std::ios::binary | std::ios::trunc);
            file << text;
        }

    private:
        fs::path mRoot;
    };

    std::shared_ptr<Mesh> MakeMesh()
    {
        return std::make_shared<Mesh>(); // GPU buffers are irrelevant to cache identity.
    }

    void TestInitializeContract()
    {
        ResourceManager resources;
        Expect(!resources.LoadTexture(kTextureKey), "load before Initialize must fail");
        Expect(!resources.RegisterMesh("Procedural/A", MakeMesh()), "register before Initialize must fail");
        Expect(!resources.FindMesh("Procedural/A"), "find before Initialize must be null");
        resources.Clear(); // allowed before Initialize

        Expect(!resources.Initialize(gDevice, fs::path("Assets")), "relative root must be rejected");
        Expect(!resources.Initialize(gDevice, kSourceAssets / "does_not_exist"), "missing root must be rejected");
        Expect(!resources.IsInitialized(), "failed Initialize must not change state");

        Expect(resources.Initialize(gDevice, kSourceAssets), "valid root must be accepted");
        Expect(!resources.Initialize(gDevice, kSourceAssets), "second Initialize must be rejected");
    }

    void TestSameKeySharesTexture()
    {
        ResourceManager resources;
        Expect(resources.Initialize(gDevice, kSourceAssets), "Initialize failed");

        auto a = resources.LoadTexture(kTextureKey);
        auto b = resources.LoadTexture(kTextureKey);
        auto c = resources.LoadTexture("./Textures/uv_checker_256.png");
        auto d = resources.LoadTexture("Textures/../Textures/uv_checker_256.png");
        auto e = resources.LoadTexture("Textures\\uv_checker_256.png");

        Expect(a != nullptr, "texture load failed");
        Expect(a.get() == b.get(), "same key must return the same texture");
        Expect(a.get() == c.get(), "'./' key must normalize to the same texture");
        Expect(a.get() == d.get(), "inner '..' key must normalize to the same texture");
        Expect(a.get() == e.get(), "backslash key must normalize to the same texture");
    }

    void TestSameKeySharesShader()
    {
        ResourceManager resources;
        Expect(resources.Initialize(gDevice, kSourceAssets), "Initialize failed");

        auto a = resources.LoadShaderPNUV(kShaderKey);
        auto b = resources.LoadShaderPNUV("./Shaders/Default.hlsl");
        Expect(a != nullptr, "shader load failed");
        Expect(a.get() == b.get(), "equivalent shader keys must share one Shader");
    }

    void TestInvalidKeysRejected()
    {
        ResourceManager resources;
        Expect(resources.Initialize(gDevice, kSourceAssets), "Initialize failed");

        const std::vector<std::string> invalid = {
            "", ".", "Textures/", "Textures/..", "../Assets/Textures/uv_checker_256.png",
            "/Textures/uv_checker_256.png", "C:/Textures/uv_checker_256.png", "C:uv_checker_256.png",
        };
        for(const auto& key : invalid) {
            Expect(!resources.LoadTexture(key), "invalid key must fail: '" + key + "'");
            Expect(!resources.RegisterMesh(key, MakeMesh()), "invalid mesh key must fail: '" + key + "'");
        }
    }

    void TestFailuresAreNotCached()
    {
        TempAssetRoot temp;
        ResourceManager resources;
        Expect(resources.Initialize(gDevice, temp.Path()), "Initialize failed");

        // Missing file, then the file appears: retry must succeed.
        Expect(!resources.LoadTexture(kTextureKey), "missing texture must fail");
        temp.CopyFromSource(kTextureKey);
        Expect(resources.LoadTexture(kTextureKey) != nullptr, "texture must load after the file appears");

        // Corrupt file, then fixed.
        temp.WriteText("Textures/broken.png", "not a png");
        Expect(!resources.LoadTexture("Textures/broken.png"), "corrupt texture must fail");
        fs::copy_file(kSourceAssets / kTextureKey, temp.Path() / "Textures/broken.png",
                      fs::copy_options::overwrite_existing);
        Expect(resources.LoadTexture("Textures/broken.png") != nullptr, "texture must load after the file is fixed");

        // Shader with a compile error, then fixed.
        temp.WriteText(kShaderKey, "this is not HLSL");
        Expect(!resources.LoadShaderPNUV(kShaderKey), "broken shader must fail");
        temp.CopyFromSource(kShaderKey);
        Expect(resources.LoadShaderPNUV(kShaderKey) != nullptr, "shader must load after the file is fixed");
    }

    void TestMeshRegistry()
    {
        ResourceManager resources;
        Expect(resources.Initialize(gDevice, kSourceAssets), "Initialize failed");

        auto first = MakeMesh();
        Expect(!resources.FindMesh("Procedural/UnitCube"), "unregistered mesh must be null");
        Expect(resources.RegisterMesh("Procedural/UnitCube", first), "register failed");
        Expect(resources.FindMesh("Procedural/UnitCube").get() == first.get(), "find must return the registered mesh");
        Expect(resources.FindMesh("./Procedural/UnitCube").get() == first.get(), "find must normalize the key");

        Expect(!resources.RegisterMesh("Procedural/UnitCube", MakeMesh()), "duplicate key must be rejected");
        Expect(!resources.RegisterMesh("Procedural/UnitCube", first), "re-registering the same mesh must be rejected");
        Expect(resources.FindMesh("Procedural/UnitCube").get() == first.get(), "duplicate must not overwrite");

        Expect(!resources.RegisterMesh("Procedural/Null", nullptr), "null mesh must be rejected");
        Expect(!resources.FindMesh("Procedural/Null"), "rejected null must not be stored");
    }

    void TestClearKeepsExternalReferences()
    {
        ResourceManager resources;
        Expect(resources.Initialize(gDevice, kSourceAssets), "Initialize failed");

        auto texture = resources.LoadTexture(kTextureKey);
        auto mesh = MakeMesh();
        Expect(resources.RegisterMesh("Procedural/UnitCube", mesh), "register failed");
        Texture2D* const before = texture.get();

        resources.Clear();

        Expect(texture.use_count() == 1, "after Clear only the external reference remains");
        Expect(texture->Width() == 256, "externally held texture must stay valid after Clear");
        Expect(!resources.FindMesh("Procedural/UnitCube"), "Clear must forget registered meshes");

        auto reloaded = resources.LoadTexture(kTextureKey);
        Expect(reloaded != nullptr, "load after Clear must work (device and root kept)");
        Expect(reloaded.get() != before, "load after Clear must create a new texture");
    }

    void TestMaterialsShareShaderAndTexture()
    {
        ResourceManager resources;
        Expect(resources.Initialize(gDevice, kSourceAssets), "Initialize failed");

        auto shader = resources.LoadShaderPNUV(kShaderKey);
        auto texture = resources.LoadTexture(kTextureKey);
        Expect(shader && texture, "resource load failed");

        auto first = std::make_unique<Material>();
        auto second = std::make_unique<Material>();
        Expect(first->Create(gDevice, shader, texture), "first material failed");
        Expect(second->Create(gDevice, shader, texture), "second material failed");
        // cache + local + 2 materials
        Expect(texture.use_count() == 4, "both materials must reference the same texture");
        Expect(shader.use_count() == 4, "both materials must reference the same shader");

        Expect(!Material().Create(gDevice, nullptr, texture), "null shader must be rejected");
        Expect(!Material().Create(gDevice, shader, nullptr), "null texture must be rejected");

        first.reset();
        Expect(texture.use_count() == 3, "destroying one material must release only its reference");
        Expect(second->Bind(gDevice.Context()), "remaining material must still bind");
    }
}

int main()
{
    if(!gDevice.Create()) {
        std::cerr << "[FAIL] D3D11 device creation failed\n";
        return 1;
    }

    using Test = std::pair<const char*, void (*)()>;
    const std::vector<Test> tests = {
        {"Initialize contract", TestInitializeContract},
        {"same key shares texture", TestSameKeySharesTexture},
        {"same key shares shader", TestSameKeySharesShader},
        {"invalid keys rejected", TestInvalidKeysRejected},
        {"failures are not cached", TestFailuresAreNotCached},
        {"mesh registry", TestMeshRegistry},
        {"Clear keeps external references", TestClearKeepsExternalReferences},
        {"materials share shader and texture", TestMaterialsShareShaderAndTexture},
    };

    int failures = 0;
    for (const auto& [name, test] : tests)
    {
        try
        {
            test();
            std::cout << "[PASS] " << name << '\n';
        }
        catch (const std::exception& error)
        {
            ++failures;
            std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
        }
    }

    if (failures == 0)
        std::cout << "All resource cache tests passed.\n";

    return failures == 0 ? 0 : 1;
}
