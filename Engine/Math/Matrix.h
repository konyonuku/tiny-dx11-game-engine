#pragma once
#include <cmath>

#include "Vector.h"

struct Matrix4x4 {
    float m[4][4] {};
    
    Matrix4x4 operator*(const Matrix4x4& r) const {
        Matrix4x4 res;
        for(int i = 0; i < 4; ++i)
            for(int j = 0; j < 4; ++j)
                for(int k = 0; k < 4; ++k)
                    res.m[i][j] += m[i][k] * r.m[k][j];
        return res;
    }

    Matrix4x4 Transposed() const {
        Matrix4x4 res;
        for(int i = 0; i < 4; ++i)
            for(int j = 0; j < 4; ++j)
                res.m[i][j] = m[j][i];
        return res;
    }
    
    static Matrix4x4 Identity() {
        Matrix4x4 res;
        for(int i = 0; i < 4; ++i) res.m[i][i] = 1.f;
        return res;
    }

    static Matrix4x4 RotationX(float rad) {
        Matrix4x4 res = Identity();
        const float c = std::cos(rad);
        const float s = std::sin(rad);
        
        res.m[1][1] = c,    res.m[1][2] = s;
        res.m[2][1] = -s,   res.m[2][2] = c;
        return res;
    }
    static Matrix4x4 RotationY(float rad) {
        Matrix4x4 res = Identity();
        const float c = std::cos(rad);
        const float s = std::sin(rad);
        
        res.m[0][0] = c,    res.m[0][2] = -s;
        res.m[2][0] = s,    res.m[2][2] = c;
        return res;
    }
    static Matrix4x4 RotationZ(float rad) {
        Matrix4x4 res = Identity();
        const float c = std::cos(rad);
        const float s = std::sin(rad);
        
        res.m[0][0] = c,    res.m[0][1] = s;
        res.m[1][0] = -s,   res.m[1][1] = c;
        return res;
    }
    
    static Matrix4x4 Scale(float x, float y, float z) {
        Matrix4x4 res;
        res.m[0][0] = x, res.m[1][1] = y, res.m[2][2] = z, res.m[3][3] = 1;
        return res;
    }

    static Matrix4x4 Translation(float x, float y, float z) {
        Matrix4x4 res = Identity();
        res.m[3][0] = x, res.m[3][1] = y, res.m[3][2] = z;
        return res;
    }
};

inline Vector4 operator*(const Vector4 v, const Matrix4x4& m) {
    return {
        v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0],
        v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1],
        v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2],
        v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3],
    };
}