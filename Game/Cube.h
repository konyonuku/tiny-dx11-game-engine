#pragma once

#include "GameFramework/Actor.h"
#include "Math/Vector.h"

class Cube : public Actor
{
public:
    void Move(const Vector3& direction, float deltaTime);
    void Rotate(float deltaTime);
    void ToggleRotation();

    void SetTransform(const Transform& transform);
    void SetRotationSpeed(const Vector3& rotationSpeed);
    void SetIsRotating(bool isRotating);
    void SetMovementSpeed(float movementSpeed);

    bool IsRotating() const;

protected:
    void Tick(float deltaTime) override;

private:
    Vector3   mRotationSpeed {};
    bool      mIsRotating = false;
    float     mMovementSpeed = 2.0f; // World units per second
};
