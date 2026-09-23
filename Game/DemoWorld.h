#pragma once
#include <optional>

#include "Math/Vector.h"
#include "Graphics/Camera.h"
#include "Graphics/Renderer.h"
#include "GameFramework/World.h"
#include "Resources/ResourceManager.h"
#include "Cube.h"
#include "DemoAssets.h"

class GraphicsDevice;


class DemoWorld
{
public:
    bool Initialize(GraphicsDevice& device);
    bool CreateCube(const Transform& transform, const Vector3& rotationSpeed, bool isRotating);
    bool SelectCube(std::size_t index);
    void MoveSelectedCube(const Vector3& direction, float deltaTime);
    void ToggleSelectedCubeRotation();
    void Update(float deltaTime);
    bool Render(Renderer& renderer, const Camera& camera);
    void Reset();

private:
    struct CubeInitialState
    {
        Transform transform;
        Vector3 rotationSpeed;
        bool isRotating = false;
        float movementSpeed = 2.0f;
    };

    void ApplySelectionScale();

    ResourceManager mResources;
    DemoAssets mAssets;
    World mActorWorld;

    std::vector<Cube*> mCubeList;
    std::vector<CubeInitialState> mInitialCubeList;
    std::optional<std::size_t> mSelectedIndex;
};
