#include "Cube.h"

void Cube::Move(const Vector3& direction, float deltaTime)
{
    mTransform.position = mTransform.position + (direction * mMovementSpeed * deltaTime);
}

void Cube::Rotate(float deltaTime)
{
    mTransform.rotation = mTransform.rotation + (mRotationSpeed * deltaTime);
}

void Cube::ToggleRotation()
{
    mIsRotating = !mIsRotating;
}

void Cube::SetTransform(const Transform& transform)
{
    mTransform = transform;
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

const Transform& Cube::GetTransform() const
{
    return mTransform;
}

bool Cube::IsRotating() const
{
    return mIsRotating;
}
