#include "Camera.h"

void Camera::SetLookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up)
{
    mView = Matrix4x4::LookAtLH(eye, target, up);
}

void Camera::SetPerspective(float fovY, float aspect, float nearZ, float farZ)
{
    mProjection = Matrix4x4::PerspectiveFovLH(fovY, aspect, nearZ, farZ);
}

const Matrix4x4 &Camera::ViewMatrix() const
{
    return mView;
}

const Matrix4x4 &Camera::ProjectionMatrix() const
{
    return mProjection;
}
