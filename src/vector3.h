#ifndef R1H_VECTOR3_H
#define R1H_VECTOR3_H

#include <cstdio>
#include <cmath>
#include "constants.h"

namespace r1h {

template<typename FPType> union Vector3 {
    struct {FPType x_, y_, z_;};
    struct {FPType x, y, z;};
    struct {FPType r, g, b;};
    struct {FPType s, t, p;};
    FPType v[3];
    
    //
    Vector3(): x(0.0), y(0.0), z(0.0) {}
    Vector3(const FPType n): x(n), y(n), z(n) {}
    Vector3(const FPType ix, const FPType iy, const FPType iz): x(ix), y(iy), z(iz) {}
    //Vector3(const FPType iv[3]): x(iv[0]), y(iv[1]), z(iv[2]){}
    
    // self operations
    inline void set(const FPType ix, const FPType iy, const FPType iz) {
        x = ix;
        y = iy;
        z = iz;
    }
    inline void set(const FPType iv[3]) {
        x = iv[0];
        y = iv[1];
        z = iv[2];
    }
    
    inline FPType length(void) const {
        return sqrt(x * x + y * y + z * z);
    }
    
    inline void normalize(void) {
        FPType l = x * x + y * y + z * z;
        if(l > VEC_EPS) {
            l = 1.0 / sqrt(l);
            x *= l;
            y *= l;
            z *= l;
        }
    }
    
    inline Vector3 negate(void) {
        x = -x;
        y = -y;
        z = -z;
    }
    
    // 2 vector operations
    static inline FPType distance(const Vector3 v0, const Vector3 v1) {
        Vector3 tmpv = v0 - v1;
        return tmpv.length();
    }
    
    static inline Vector3 normalized(const Vector3 v) {
        Vector3 ret = v;
        ret.normalize();
        return ret;
    }
    
    static inline Vector3 negated(const Vector3 v) {
        Vector3 retv(-v.x, -v.y, -v.z);
        return retv;
    }
    static inline Vector3 mul(const Vector3 v0, const Vector3 v1) {
        Vector3 retv;
        retv.x = v0.x * v1.x;
        retv.y = v0.y * v1.y;
        retv.z = v0.z * v1.z;
        return retv;
    }
    static inline Vector3 div(const Vector3 v0, const Vector3 v1) {
        Vector3 retv;
        retv.x = v0.x / v1.x;
        retv.y = v0.y / v1.y;
        retv.z = v0.z / v1.z;
        return retv;
    }
    
    static inline FPType dot(const Vector3 v0, const Vector3 v1) {
        return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
    }
    static inline Vector3 cross(const Vector3 v0, const Vector3 v1) {
        Vector3 retv;
        retv.x = v0.y * v1.z - v0.z * v1.y;
        retv.y = v0.z * v1.x - v0.x * v1.z;
        retv.z = v0.x * v1.y - v0.y * v1.x;
        return retv;
    }
    
    static inline Vector3 lerp(const Vector3 v0, const Vector3 v1, const FPType t) {
        Vector3 retv;
        FPType t0 = 1.0 - t;
        retv.x = v0.x * t0 + v1.x * t;
        retv.y = v0.y * t0 + v1.y * t;
        retv.z = v0.z * t0 + v1.z * t;
        return retv;
    }
   static inline  Vector3 project(const Vector3 v0, const Vector3 v1) {
        Vector3 nv = Vector3::normalized(v1);
        FPType d = Vector3::dot(v0, nv);
        return nv + d;
    }
    
    // util
    static void sprint(char *buf, const Vector3 v) {
        sprintf(buf, "v3(%.4f,%.4f,%.4f)", v.x, v.y, v.z);
    }
    
    // operators
    inline Vector3 operator+(const Vector3 &b) const {
        return Vector3(x + b.x, y + b.y, z + b.z);
    }
    inline Vector3 operator-(const Vector3 &b) const {
        return Vector3(x - b.x, y - b.y, z - b.z);
    }
    
    inline Vector3 operator*(const FPType s) const {
        return Vector3(x * s, y * s, z * s);
    }
    
    inline Vector3 operator/(const FPType s) const {
        return Vector3(x / s, y / s, z / s);
    }
    
    inline Vector3 operator+=(const Vector3 &b) {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }
};

template<typename FPType> inline const Vector3<FPType> operator*(const FPType s, const Vector3<FPType> &v) {
    return v * s;
}

template<typename FPType> inline const Vector3<FPType> operator/(const FPType s, const Vector3<FPType> &v) {
    return v / s;
}

}
#endif
