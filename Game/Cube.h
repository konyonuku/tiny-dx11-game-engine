#pragma once

#include "Math/Transform.h"
#include "Math/Vector.h"

class Cube
{
public:
    void Move(const Vector3& direction, float deltaTime);
    void Rotate(float deltaTime);
    void ToggleRotation();

    void SetTransform(const Transform& transform);
    void SetRotationSpeed(const Vector3& rotationSpeed);
    void SetIsRotating(bool isRotating);
    void SetMovementSpeed(float movementSpeed);

    const Transform& GetTransform() const;
    bool IsRotating() const;

private:
    Transform mTransform;
    Vector3   mRotationSpeed;
    bool      mIsRotating = false;
    float     mMovementSpeed = 2.0f; // World units per second
};
