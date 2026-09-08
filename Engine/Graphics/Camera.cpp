#include "Camera.h"

void Camera::SetLookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up)
{
    mView = Matrix4x4::LookAtLH(eye, target, up);
    mPosition = eye;
}

void Camera::SetPerspective(float fovY, float aspect, float nearZ, float farZ)
{
    mProjection = Matrix4x4::PerspectiveFovLH(fovY, aspect, nearZ, farZ);
    RebuildProjection();
}

void Camera::SetAspect(float aspect)
{
    if(aspect <= 0.0f) return;
    mAspect = aspect;
    RebuildProjection();
}

const Matrix4x4 &Camera::ViewMatrix() const
{
    return mView;
}

const Matrix4x4 &Camera::ProjectionMatrix() const
{
    return mProjection;
}

void Camera::RebuildProjection()
{
    mProjection = Matrix4x4::PerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
}
