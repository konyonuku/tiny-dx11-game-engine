#pragma once

#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/MathUtils.h"


class Camera {
    public:
        void SetLookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
        void SetPerspective(float fovY, float aspect, float nearZ, float farZ);
        void SetAspect(float aspect);
        void RebuildProjection();
        const Matrix4x4& ViewMatrix() const;
        const Matrix4x4& ProjectionMatrix() const;
        const Vector3& Position() const { return mPosition; }

    private:
        Matrix4x4 mView         = Matrix4x4::Identity();
        Matrix4x4 mProjection   = Matrix4x4::Identity();
        Vector3   mPosition {};

        float mFovY   = ToRadian(60.f);
        float mAspect = 16.0f / 9.0f;
        float mNearZ  = 0.1f;
        float mFarZ   = 100.0f;
        /*
        constexpr float      pi = 3.1415926535f;
        mCameraAngle = -pi / 2;
        const float aspect = static_cast<float>(GetWindow().ClientWidth())/static_cast<float>(GetWindow().ClientHeight());
        mCamera.SetPerspective(60.f*pi/180.f, aspect, 0.1f, 100.f);
        */
};