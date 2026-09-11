#pragma once

#include <cstddef>
#include <optional>
#include <vector>

#include "Math/Vector.h"
#include "Graphics/Camera.h"
#include "Graphics/Renderer.h"

#include "Cube.h"
#include "CubeRenderer.h"


class GraphicsDevice;

class World
{
public:
    bool Initialize(GraphicsDevice& device);
    void CreateCube(const Transform& transform, const Vector3& rotationSpeed, bool isRotating);
    bool SelectCube(std::size_t index);
    void MoveSelectedCube(const Vector3& direction, float deltaTime);
    void ToggleSelectedCubeRotation();
    void Update(float deltaTime);
    void Render(Renderer& renderer, const Camera& camera);
    void Reset();

private:
    std::vector<Cube> mCubeList;
    std::vector<Cube> mInitialCubeList;
    std::optional<std::size_t> mSelectedIndex;

    CubeRenderer mCubeRenderer;
};
