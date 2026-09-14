#include "DemoWorld.h"

#include "Core/Log.h"

namespace
{
    constexpr float kSelectedScaleMultiplier = 1.2f;
}

bool DemoWorld::Initialize(GraphicsDevice& device)
{
    if(!mCubeRenderer.Create(device)) return false;
    Core::LogInfo("World Initialized.");
    return true;
}

void DemoWorld::CreateCube(const Transform& transform, const Vector3& rotationSpeed, bool isRotating)
{
    Cube cube;
    cube.SetTransform(transform);
    cube.SetRotationSpeed(rotationSpeed);
    cube.SetIsRotating(isRotating);

    mCubeList.push_back(cube);
    mInitialCubeList.push_back(cube);

    if(!mSelectedIndex.has_value()) {
        mSelectedIndex = 0;
    }
}

bool DemoWorld::SelectCube(std::size_t index)
{
    if(index >= mCubeList.size()) return false;

    mSelectedIndex = index;
    return true;
}

void DemoWorld::MoveSelectedCube(const Vector3& direction, float deltaTime)
{
    if(!mSelectedIndex.has_value() || *mSelectedIndex >= mCubeList.size()) return;
    mCubeList[*mSelectedIndex].Move(direction, deltaTime);
}

void DemoWorld::ToggleSelectedCubeRotation()
{
    if(!mSelectedIndex.has_value() || *mSelectedIndex >= mCubeList.size()) return;
    mCubeList[*mSelectedIndex].ToggleRotation();
}

void DemoWorld::Update(float deltaTime)
{
    for(auto& c : mCubeList) 
        if(c.IsRotating()) 
            c.Rotate(deltaTime);
}

void DemoWorld::Render(Renderer& renderer, const Camera& camera)
{
    const Matrix4x4& view = camera.ViewMatrix();
    const Matrix4x4& projection = camera.ProjectionMatrix();

    for(std::size_t i = 0; i < mCubeList.size(); ++i) {
        Transform renderTransform = mCubeList[i].GetTransform();
        if(mSelectedIndex == i) {
            renderTransform.scale = renderTransform.scale * kSelectedScaleMultiplier;
        }

        const Matrix4x4 world = renderTransform.WorldMatrix();
        const Matrix4x4 wvp = world * view * projection;

        mCubeRenderer.Render(renderer.Context(), world, wvp, camera.Position());
    }
}

void DemoWorld::Reset()
{
    mCubeList = mInitialCubeList;
    mSelectedIndex = mCubeList.empty() ? std::optional<std::size_t>{} : std::optional<std::size_t>{0};
}
