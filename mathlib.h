#ifndef MATHLIB_H
#define MATHLIB_H

#include <stdio.h>
#include <math.h>

namespace mlib {

    struct Vec2 { float x, y; };
    struct Vec3 { float x, y, z; };
    struct Vec4 { float x, y, z, w; };

    struct Mat2 { float m[4]; };
    struct Mat3 { float m[9]; };
    struct Mat4 { float m[16]; };

    inline Vec2 SumVec2(Vec2 a, Vec2 b)   { return {a.x+b.x, a.y+b.y}; }
    inline Vec3 SumVec3(Vec3 a, Vec3 b)   { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
    inline Vec4 SumVec4(Vec4 a, Vec4 b)   { return {a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w}; }

    inline Vec2 SubsVec2(Vec2 a, Vec2 b)  { return {a.x-b.x, a.y-b.y}; }
    inline Vec3 SubsVec3(Vec3 a, Vec3 b)  { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
    inline Vec4 SubsVec4(Vec4 a, Vec4 b)  { return {a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w}; }

    inline Vec2 Vec2XSca(Vec2 a, float k) { return {a.x*k, a.y*k}; }
    inline Vec3 Vec3XSca(Vec3 a, float k) { return {a.x*k, a.y*k, a.z*k}; }
    inline Vec4 Vec4XSca(Vec4 a, float k) { return {a.x*k, a.y*k, a.z*k, a.w*k}; }

    inline Vec2 Vec2DivSca(Vec2 a, float k) { return {a.x/k, a.y/k}; }
    inline Vec3 Vec3DivSca(Vec3 a, float k) { return {a.x/k, a.y/k, a.z/k}; }
    inline Vec4 Vec4DivSca(Vec4 a, float k) { return {a.x/k, a.y/k, a.z/k, a.w/k}; }

    inline float Vec2Magn(Vec2 a) { return sqrtf(a.x*a.x + a.y*a.y); }
    inline float Vec3Magn(Vec3 a) { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z); }
    inline float Vec4Magn(Vec4 a) { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w); }

    inline void Vec2Print(Vec2 a) { printf("[X]: %f [Y]: %f\n", a.x, a.y); }
    inline void Vec3Print(Vec3 a) { printf("[X]: %f [Y]: %f [Z]: %f\n", a.x, a.y, a.z); }
    inline void Vec4Print(Vec4 a) { printf("[X]: %f [Y]: %f [Z]: %f [W]: %f\n", a.x, a.y, a.z, a.w); }

    inline Vec2 Vec2Norm(Vec2 a) { return Vec2DivSca(a, Vec2Magn(a)); }
    inline Vec3 Vec3Norm(Vec3 a) { return Vec3DivSca(a, Vec3Magn(a)); }
    inline Vec4 Vec4Norm(Vec4 a) { return Vec4DivSca(a, Vec4Magn(a)); }

    inline bool IsVec2Norm(Vec2 a) { float m = Vec2Magn(a); return m >= 0.9998f && m <= 1.0002f; }
    inline bool IsVec3Norm(Vec3 a) { float m = Vec3Magn(a); return m >= 0.9998f && m <= 1.0002f; }
    inline bool IsVec4Norm(Vec4 a) { float m = Vec4Magn(a); return m >= 0.9998f && m <= 1.0002f; }

    inline Vec2 Perpend2D_1(Vec2 a) { return { a.y, -a.x}; }
    inline Vec2 Perpend2D_2(Vec2 a) { return {-a.y,  a.x}; }
}

#endif