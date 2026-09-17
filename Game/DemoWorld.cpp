#include "DemoWorld.h"

#include "Core/Log.h"
#include "GameFramework/StaticMeshComponent.h"
#include "Graphics/RenderTypes.h"


namespace
{
    constexpr float kSelectedScaleMultiplier = 1.2f;
}

bool DemoWorld::Initialize(GraphicsDevice& device)
{
    if(!mAssets.Create(device)) return false;
    Core::LogInfo("Actor demo initialized.");
    return true;
}

bool DemoWorld::CreateCube(const Transform& transform, const Vector3& rotationSpeed, bool isRotating)
{
    if(!mAssets.cubeMesh || !mAssets.material) return false;

    Cube* cube = mActorWorld.SpawnActor<Cube>();
    if(!cube) return false;

    auto* meshComponent = cube->AddComponent<StaticMeshComponent>();
    if(!meshComponent) {
        mActorWorld.DestroyActor(cube);
        return false;
    }

    const CubeInitialState initial {transform, rotationSpeed, isRotating, 2.0f};
    cube->SetTransform(initial.transform);
    cube->SetRotationSpeed(initial.rotationSpeed);
    cube->SetIsRotating(initial.isRotating);
    cube->SetMovementSpeed(initial.movementSpeed);
    meshComponent->SetMesh(mAssets.cubeMesh);
    meshComponent->SetMaterial(mAssets.material);

    mCubeList.push_back(cube);
    mInitialCubeList.push_back(initial);

    if(!mSelectedIndex.has_value()) {
        mSelectedIndex = 0;
    }
    ApplySelectionScale();
    return true;
}

bool DemoWorld::SelectCube(std::size_t index)
{
    if(index >= mCubeList.size()) return false;

    mSelectedIndex = index;
    ApplySelectionScale();
    return true;
}

void DemoWorld::MoveSelectedCube(const Vector3& direction, float deltaTime)
{
    if(!mSelectedIndex.has_value() || *mSelectedIndex >= mCubeList.size()) return;
    mCubeList[*mSelectedIndex]->Move(direction, deltaTime);
}

void DemoWorld::ToggleSelectedCubeRotation()
{
    if(!mSelectedIndex.has_value() || *mSelectedIndex >= mCubeList.size()) return;
    mCubeList[*mSelectedIndex]->ToggleRotation();
}

void DemoWorld::Update(float deltaTime)
{
    mActorWorld.Tick(deltaTime);
}

bool DemoWorld::Render(Renderer& renderer, const Camera& camera)
{
    FrameConstants frame {};
    frame.viewProjection = camera.ViewMatrix() * camera.ProjectionMatrix();
    frame.lightDirection = {1.0f, -1.0f, 1.0f};
    frame.lightIntensity = 1.0f;
    frame.lightColor = {1.0f, 1.0f, 1.0f};
    frame.ambientIntensity = 0.05f;
    frame.cameraPosition = camera.Position();

    if(!renderer.SetFrameConstants(frame)) return false;
    return mActorWorld.Render(renderer);
}

void DemoWorld::Reset()
{
    for(std::size_t i = 0; i < mCubeList.size(); ++i) {
        const auto& initial = mInitialCubeList[i];
        Cube* cube = mCubeList[i];
        cube->SetTransform(initial.transform);
        cube->SetRotationSpeed(initial.rotationSpeed);
        cube->SetIsRotating(initial.isRotating);
        cube->SetMovementSpeed(initial.movementSpeed);
    }
    mSelectedIndex = mCubeList.empty() ? std::optional<std::size_t>{} : std::optional<std::size_t>{0};
    ApplySelectionScale();
}

void DemoWorld::ApplySelectionScale()
{
    for(std::size_t i = 0; i < mCubeList.size(); ++i) {
        const Vector3 baseScale = mInitialCubeList[i].transform.scale;
        mCubeList[i]->GetTransform().scale =
            mSelectedIndex == i ? baseScale * kSelectedScaleMultiplier : baseScale;
    }
}
