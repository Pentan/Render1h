#ifndef R1H_VECTOR4_H
#define R1H_VECTOR4_H

#include <cstdio>
#include <cmath>
#include "constants.h"
#include "vector3.h"

namespace r1h {

template<typename FPType> union Vector4 {
    struct {FPType x, y, z, w;};
    struct {FPType r, g, b, a;};
    struct {FPType s, t, p, q;};
    FPType v[4];

    //
    Vector4(): x(0.0), y(0.0), z(0.0), w(0.0) {}
    Vector4(const FPType n): x(n), y(n), z(n), w(n) {}
    Vector4(const FPType ix, const FPType iy, const FPType iz, const FPType iw): x(ix), y(iy), z(iz), w(iw) {}
    
    // self operation
    inline void set(const FPType ix, const FPType iy, const FPType iz, const FPType iw) {
        x = ix;
        y = iy;
        z = iz;
        w = iw;
    }
    inline void set(const FPType iv[4]) {
        x = iv[0];
        y = iv[1];
        z = iv[2];
        w = iv[3];
    }
    inline void set(const Vector3<FPType> vxyz, const FPType iw) {
        x = vxyz.x;
        y = vxyz.y;
        z = vxyz.z;
        w = iw;
    }
    
    inline Vector3<FPType> getXYZ(void) const {
        return Vector3<FPType>(x, y, z);
    }
    inline FPType length(void) const {
        return sqrt(x * x + y * y + z * z + w * w);
    }
    
    inline void normalize(void) {
        FPType l = x * x + y * y + z * z + w * w;
        if(l > VEC_EPS) {
            l = 1.0 / sqrt(l);
            x *= l;
            y *= l;
            z *= l;
            w *= l;
        }
    }
    inline void negate(void) {
        x = -x;
        y = -y;
        z = -z;
        w = -w;
    }

    // 2 vector
    static inline FPType distance(const Vector4 v0, const Vector4 v1) {
        Vector4 tmpv = v1 - v0;
        return tmpv.length();
    }
    static inline Vector4 normalized(const Vector4 v) {
        Vector4 retv = v;
        retv.normalize();
        return retv;
    }
    static inline Vector4 negated(const Vector4 v) {
        return Vector4(-v.x, -v.y, -v.z, -v.w);
    }
    static inline Vector4 mul(const Vector4 v0, const Vector4 v1) {
        Vector4 retv;
        retv.x = v0.x * v1.x;
        retv.y = v0.y * v1.y;
        retv.z = v0.z * v1.z;
        retv.w = v0.w * v1.w;
        return retv;
    }
    static inline Vector4 div(const Vector4 v0, const Vector4 v1) {
        Vector4 retv;
        retv.x = v0.x / v1.x;
        retv.y = v0.y / v1.y;
        retv.z = v0.z / v1.z;
        retv.w = v0.w / v1.w;
        return retv;
    }
    
    static inline FPType dot(const Vector4 v0, const Vector4 v1) {
        return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w;
    }
    static inline Vector4 cross(const Vector4 v0, const Vector4 v1) {
        Vector3<FPType> cv = Vector3<FPType>::cross(v0.getXYZ(), v1.getXYZ());
        return Vector4(cv, 0.0);
    }
    
    static inline Vector4 lerp(const Vector4 v0, const Vector4 v1, FPType t) {
        Vector4 retv;
        FPType t0 = 1.0f - t;
        retv.x = v0.x * t0 + v1.x * t;
        retv.y = v0.y * t0 + v1.y * t;
        retv.z = v0.z * t0 + v1.z * t;
        retv.w = v0.w * t0 + v1.w * t;
        return retv;
    }
    static inline Vector4 project(const Vector4 v0, const Vector4 v1) {
        Vector4 nv = Vector4::normalized(v1);
        FPType d = Vector4::dot(v0, v1);
        return nv * d;
    }
    
    // util
    static void sprint(char *buf, const Vector4 v) {
        sprintf(buf, "v4(%.4f,%.4f,%.4f,%.4f)", v.x, v.y, v.z, v.w);
    }
    
    // operators
    inline Vector4 operator+(const Vector4 &v) {
        return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
    }
    inline Vector4 operator-(const Vector4 &v) {
        return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
    }
    inline Vector4 operator*(const FPType s) {
        return Vector4(v.x * s, v.y * s, v.z * s, v.w * s);
    }
    inline Vector4 operator/(const FPType s) {
        return Vector4(v.x / s, v.y / s, v.z / s, v.w / s);
    }
    inline Vector4 operator+=(const Vector4 &b) {
        x += b.x;
        y += b.y;
        z += b.z;
        w += b.w;
        return *this;
    }
};

template<typename FPType> inline const Vector4<FPType> operator*(const FPType s, const Vector4<FPType> &v) {
    return v * s;
}

template<typename FPType> inline const Vector4<FPType> operator/(const FPType s, const Vector4<FPType> &v) {
    return v / s;
}

}
#endif
