#pragma once

#include "Vector.h"
#include "Matrix.h"


struct Transform {
    Vector3 position {0, 0, 0};
    Vector3 rotation {0, 0, 0};
    Vector3 scale    {1, 1, 1};

    Matrix4x4 WorldMatrix() const {
        Matrix4x4 s  = Matrix4x4::Scale(scale.x, scale.y, scale.z);
        Matrix4x4 rx = Matrix4x4::RotationX(rotation.x);
        Matrix4x4 ry = Matrix4x4::RotationY(rotation.y);
        Matrix4x4 rz = Matrix4x4::RotationZ(rotation.z);
        Matrix4x4 t  = Matrix4x4::Translation(position.x, position.y, position.z);

        return (s * rx * ry * rz * t);
    }
};
    // ObjectConstant ojc{};
    // ojc.world = Matrix4x4::Identity().Transposed();
    // ojc.world = Matrix4x4::Scale(0.5f, 0.5f, 1.0f).Transposed();
    // ojc.world = Matrix4x4::Translation(0.3f, 0.3f, 0.f).Transposed();