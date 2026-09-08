#pragma once

inline constexpr float PI = 3.1415926535f; //PI(rad)
inline constexpr float ToRadian(float degree) { return degree * (PI / 180.0f);}
inline constexpr float ToDegree(float radian) { return radian * (180.0f / PI);}