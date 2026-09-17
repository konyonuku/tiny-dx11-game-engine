#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "GameFramework/StaticMeshComponent.h"
#include "GameFramework/World.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Renderer.h"

namespace
{
    void Expect(bool condition, const std::string& message)
    {
        if(!condition) throw std::runtime_error(message);
    }

    void ExpectMatrix(const Matrix4x4& actual, const Matrix4x4& expected)
    {
        for(int row = 0; row < 4; ++row)
            for(int column = 0; column < 4; ++column)
                Expect(std::abs(actual.m[row][column] - expected.m[row][column]) < 0.00001f,
                       "collected matrix differs from owner transform");
    }

    // Use the real World/Actor dispatch and StaticMeshComponent collection code.
    // Capture items here instead of forwarding them to GPU drawing. Mesh/Material
    // are deliberately uninitialized: this suite checks collection, not pixels.
    class CapturingMeshComponent final : public StaticMeshComponent
    {
    public:
        explicit CapturingMeshComponent(std::vector<RenderItem>& captured)
            : mCaptured(captured) {}

    protected:
        void CollectRenderItems(std::vector<RenderItem>&) const override
        {
            StaticMeshComponent::CollectRenderItems(mCaptured);
        }

    private:
        std::vector<RenderItem>& mCaptured;
    };

    class CallbackActor final : public Actor
    {
    public:
        std::function<void()> onTick;
        std::function<void()> onEndPlay;

    protected:
        void Tick(float) override { if(onTick) onTick(); }
        void EndPlay() override { if(onEndPlay) onEndPlay(); }
    };

    CapturingMeshComponent* Attach(Actor& actor, std::vector<RenderItem>& captured,
                                    const std::shared_ptr<Mesh>& mesh,
                                    const std::shared_ptr<Material>& material)
    {
        auto* component = actor.AddComponent<CapturingMeshComponent>(captured);
        Expect(component != nullptr, "component attachment failed");
        component->SetMesh(mesh);
        component->SetMaterial(material);
        return component;
    }

    void TestLifecycleAndTransformSnapshots()
    {
        std::vector<RenderItem> captured;
        auto mesh = std::make_shared<Mesh>();
        auto material = std::make_shared<Material>();
        Renderer renderer;
        World world;
        auto* first = world.SpawnActor<Actor>();
        auto* second = world.SpawnActor<Actor>();
        Attach(*first, captured, mesh, material);
        Attach(*second, captured, mesh, material);
        first->GetTransform().position = {2.0f, 3.0f, 4.0f};
        first->GetTransform().rotation = {0.2f, 0.3f, 0.4f};
        first->GetTransform().scale = {1.2f, 1.2f, 1.2f};
        second->GetTransform().position = {-2.0f, 0.0f, 1.0f};

        Expect(world.Render(renderer) && captured.empty(), "pending actors were collected");
        world.Tick(0.0f);
        Expect(world.Render(renderer), "capture-only render failed");
        Expect(captured.size() == 2, "active actors did not yield two items");
        Expect(captured[0].mesh == mesh && captured[1].mesh == mesh,
               "actors did not share their mesh");
        Expect(captured[0].material == material && captured[1].material == material,
               "actors did not share their material");
        const Matrix4x4 snapshot = first->GetTransform().WorldMatrix();
        ExpectMatrix(captured[0].world, snapshot);
        ExpectMatrix(captured[1].world, second->GetTransform().WorldMatrix());

        first->GetTransform().position.x = 8.0f;
        ExpectMatrix(captured[0].world, snapshot);
        captured.clear();
        Expect(world.Render(renderer), "second collection failed");
        ExpectMatrix(captured[0].world, first->GetTransform().WorldMatrix());

        world.DestroyActor(first);
        captured.clear();
        Expect(world.Render(renderer) && captured.size() == 1,
               "pending-destroy actor was collected");
        ExpectMatrix(captured[0].world, second->GetTransform().WorldMatrix());
        world.Tick(0.0f);
        captured.clear();
        Expect(world.Render(renderer) && captured.size() == 1,
               "destroying one actor affected the surviving actor");
        world.Clear();
        captured.clear();
        Expect(world.Render(renderer) && captured.empty(), "Clear left render items");
    }

    void TestVisibilityMissingAssetsAndMultipleComponents()
    {
        std::vector<RenderItem> captured;
        auto mesh = std::make_shared<Mesh>();
        auto material = std::make_shared<Material>();
        Renderer renderer;
        World world;
        auto* actor = world.SpawnActor<Actor>();
        actor->GetTransform().position = {3.0f, 0.0f, -1.0f};
        Attach(*actor, captured, mesh, material);
        Attach(*actor, captured, mesh, material);
        auto* hidden = Attach(*actor, captured, mesh, material);
        hidden->SetVisible(false);
        auto* missingMesh = Attach(*actor, captured, nullptr, material);
        auto* missingMaterial = Attach(*actor, captured, mesh, nullptr);
        world.SpawnActor<Actor>(); // An actor without a mesh contributes nothing.
        world.Tick(0.0f);

        Expect(world.Render(renderer) && captured.size() == 2,
               "hidden/null components contributed items or valid components were lost");
        ExpectMatrix(captured[0].world, actor->GetTransform().WorldMatrix());
        ExpectMatrix(captured[1].world, actor->GetTransform().WorldMatrix());

        hidden->SetVisible(true);
        missingMesh->SetMesh(mesh);
        missingMaterial->SetMaterial(material);
        captured.clear();
        Expect(world.Render(renderer) && captured.size() == 5,
               "restored components were not collected");
    }

    void TestSpawnDuringTickAndRenderGuards()
    {
        std::vector<RenderItem> captured;
        auto mesh = std::make_shared<Mesh>();
        auto material = std::make_shared<Material>();
        Renderer renderer;
        Actor* spawned = nullptr;
        bool tickRenderRejected = false;
        bool clearRenderRejected = false;
        World world;
        auto* driver = world.SpawnActor<CallbackActor>();
        driver->onTick = [&]() {
            tickRenderRejected = !world.Render(renderer);
            if(!spawned) {
                spawned = world.SpawnActor<Actor>();
                Attach(*spawned, captured, mesh, material);
            }
        };
        driver->onEndPlay = [&]() { clearRenderRejected = !world.Render(renderer); };

        world.Tick(0.0f);
        Expect(tickRenderRejected, "Render was allowed inside Tick");
        Expect(spawned != nullptr, "Tick did not spawn its actor");
        Expect(world.Render(renderer) && captured.empty(), "Tick spawn rendered too early");
        world.Tick(0.0f);
        Expect(world.Render(renderer) && captured.size() == 1,
               "Tick spawn was not collected on the next frame");
        world.Clear();
        Expect(clearRenderRejected, "Render was allowed inside Clear");
    }

    void TestSharedResourceLifetime()
    {
        std::vector<RenderItem> captured;
        auto mesh = std::make_shared<Mesh>();
        auto material = std::make_shared<Material>();
        std::weak_ptr<Mesh> weakMesh = mesh;
        std::weak_ptr<Material> weakMaterial = material;
        Renderer renderer;
        World world;
        auto* first = world.SpawnActor<Actor>();
        auto* second = world.SpawnActor<Actor>();
        Attach(*first, captured, mesh, material);
        Attach(*second, captured, mesh, material);
        mesh.reset();
        material.reset();
        world.Tick(0.0f);
        world.DestroyActor(first);
        world.Tick(0.0f);
        Expect(!weakMesh.expired() && !weakMaterial.expired(),
               "deleting one actor destroyed shared resources");

        Expect(world.Render(renderer) && captured.size() == 1, "survivor did not collect");
        world.Clear();
        Expect(!weakMesh.expired() && !weakMaterial.expired(),
               "render snapshot did not retain shared resources");
        captured.clear();
        Expect(weakMesh.expired() && weakMaterial.expired(), "resources leaked after last owner");
    }

    void TestDrawFailureWithoutFrameConstants()
    {
        Renderer renderer; // DrawMesh must reject before touching a D3D context.
        World world;
        auto* actor = world.SpawnActor<Actor>();
        auto* component = actor->AddComponent<StaticMeshComponent>();
        component->SetMesh(std::make_shared<Mesh>());
        component->SetMaterial(std::make_shared<Material>());
        Expect(world.Render(renderer), "unstarted actor should leave an empty render");
        world.Tick(0.0f);
        Expect(!world.Render(renderer), "World swallowed DrawMesh failure");
        component->SetVisible(false);
        Expect(world.Render(renderer), "hidden component reached DrawMesh");
    }
}

int main()
{
    using Test = std::pair<const char*, void (*)()>;
    const std::vector<Test> tests = {
        {"lifecycle and transform snapshots", TestLifecycleAndTransformSnapshots},
        {"visibility, missing assets, multiple components", TestVisibilityMissingAssetsAndMultipleComponents},
        {"deferred spawn and render guards", TestSpawnDuringTickAndRenderGuards},
        {"shared resource lifetime", TestSharedResourceLifetime},
        {"DrawMesh failure propagation", TestDrawFailureWithoutFrameConstants},
    };

    int failures = 0;
    for(const auto& [name, test] : tests) {
        try {
            test();
            std::cout << "[PASS] " << name << '\n';
        } catch(const std::exception& error) {
            ++failures;
            std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
        }
    }
    return failures == 0 ? 0 : 1;
}
