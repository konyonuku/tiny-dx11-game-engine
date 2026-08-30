#pragma once

#include "Math/Vector.h"
#include "Math/Matrix.h"


class Camera {
    public:
        void SetLookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
        void SetPerspective(float fovY, float aspect, float nearZ, float farZ);
        const Matrix4x4& ViewMatrix() const;
        const Matrix4x4& ProjectionMatrix() const;
    private:
        Matrix4x4 mView         = Matrix4x4::Identity();
        Matrix4x4 mProjection   = Matrix4x4::Identity();
};