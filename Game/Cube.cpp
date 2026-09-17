#include "Cube.h"

void Cube::Move(const Vector3& direction, float deltaTime)
{
    auto& transform = GetTransform();
    transform.position = transform.position + (direction * mMovementSpeed * deltaTime);
}

void Cube::Rotate(float deltaTime)
{
    auto& transform = GetTransform();
    transform.rotation = transform.rotation + (mRotationSpeed * deltaTime);
}

void Cube::ToggleRotation()
{
    mIsRotating = !mIsRotating;
}

void Cube::SetTransform(const Transform& transform)
{
    GetTransform() = transform;
}

void Cube::SetRotationSpeed(const Vector3& rotationSpeed)
{
    mRotationSpeed = rotationSpeed;
}

void Cube::SetIsRotating(bool isRotating)
{
    mIsRotating = isRotating;
}

void Cube::SetMovementSpeed(float movementSpeed)
{
    mMovementSpeed = movementSpeed;
}

void Cube::Tick(float deltaTime)
{
    if(mIsRotating) Rotate(deltaTime);
}

bool Cube::IsRotating() const
{
    return mIsRotating;
}
