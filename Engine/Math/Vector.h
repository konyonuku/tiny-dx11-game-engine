#pragma once
#include <cmath>

struct Vector3 {
    float x=0.f,y=0.f,z=0.f;

    Vector3 operator+(const Vector3& r) const { return { x+r.x, y+r.y, z+r.z }; }
    Vector3 operator-(const Vector3& r) const { return { x-r.x, y-r.y, z-r.z }; }
    Vector3 operator*(float a) const { return { x*a, y*a, z*a }; }
    Vector3 operator/(float a) const { return { x/a, y/a, z/a }; }
    
    float Dot(const Vector3& v) const { return x*v.x + y*v.y + z*v.z; }
    Vector3 Cross(const Vector3& v) const { return {y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x}; }
    float LengthSquared() const { return Dot(*this); }
    float Length() const { return std::sqrt(LengthSquared()); }

    Vector3 Normalized() const {
        constexpr float e = 0.000001f;
        const float lengthSquared = LengthSquared();
        if(lengthSquared <= e*e) return {0.f,0.f,0.f};
        return *this/std::sqrt(lengthSquared);
    }
};

struct Vector4 {
    float x=0.f,y=0.f,z=0.f,w=0.f;

    Vector4 operator+(const Vector4& r) const { return { x+r.x, y+r.y, z+r.z, w+r.w }; }
    Vector4 operator-(const Vector4& r) const { return { x-r.x, y-r.y, z-r.z, w-r.w }; }
    Vector4 operator*(float a) const { return { x*a, y*a, z*a, w*a }; }
    Vector4 operator/(float a) const { return { x/a, y/a, z/a, w/a }; }

    float Dot(const Vector4& v) const { return x*v.x+y*v.y+z*v.z+w*v.w; }
    float LengthSquared() const { return Dot(*this); }
    float Length() const { return std::sqrt(LengthSquared()); }

    Vector4 Normalized() const {
        constexpr float e = 0.000001f;
        const float lengthSquared = LengthSquared();
        if(lengthSquared <= e*e) return {0.f,0.f,0.f,0.f};
        return *this/std::sqrt(lengthSquared);
    }
};

inline Vector3 operator*(float a, const Vector3& v) { return v*a; }
inline Vector4 operator*(float a, const Vector4& v) { return v*a; }
