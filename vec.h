#ifndef R1H_VEC_H
#define R1H_VEC_H

#include <cstdio>
#include <cmath>

namespace r1h {

struct Vec {
    double x_, y_, z_;
    
    Vec(): x_(0.0), y_(0.0), z_(0.0) {};
    Vec(const double x): x_(x), y_(x), z_(x) {};
    Vec(const double x, const double y, const double z): x_(x), y_(y), z_(z) {};
    
    inline void print(const char *msg, const bool br=true) const {
        printf("%s:(%lf,%lf,%lf)", msg, x_, y_, z_);
        if(br) printf("\n");
    }
    
    inline Vec operator+=(const Vec &b) {
        x_ += b.x_;
        y_ += b.y_;
        z_ += b.z_;
        return *this;
    }
    
    inline Vec operator+(const Vec &b) const {
        return Vec(x_ + b.x_, y_ + b.y_, z_ + b.z_);
    }
    inline Vec operator-(const Vec &b) const {
        return Vec(x_ - b.x_, y_ - b.y_, z_ - b.z_);
    }
    inline Vec operator*(const double b) const {
        return Vec(x_ * b, y_ * b, z_ * b);
    }
    inline Vec operator/(const double b) const {
        return Vec(x_ / b, y_ / b, z_ / b);
    }
    
    inline const double length_squared() const {
        return x_ * x_ +  y_ * y_ + z_ * z_;
    }
    inline const double length() const {
        return sqrt(length_squared());
    }
};
inline Vec operator*(double f, const Vec &v) {
    return v * f;
}
inline Vec normalize(const Vec &v) {
    return v / v.length();
}
inline const Vec multiply(const Vec &v1, const Vec &v2) {
    return Vec(v1.x_ * v2.x_, v1.y_ * v2.y_, v1.z_ * v2.z_);
}
inline const double dot(const Vec &v1, const Vec &v2) {
    return v1.x_ * v2.x_ + v1.y_ * v2.y_ + v1.z_ * v2.z_;
}
inline const Vec cross(const Vec &v1, const Vec &v2) {
    return Vec(
        (v1.y_ * v2.z_) - (v1.z_ * v2.y_),
        (v1.z_ * v2.x_) - (v1.x_ * v2.z_),
        (v1.x_ * v2.y_) - (v1.y_ * v2.x_));
}

}

/*
#include "vqmvector3.h"

namespace r1h {

typedef double vecfp_t;
typedef Vector3<vecfp_t> Vec;

//+++++
inline Vec normalize(const Vec &v) {
    return Vector3<vecfp_t>::normalized(v);
}
inline const Vec multiply(const Vec &v1, const Vec &v2) {
    return Vector3<vecfp_t>::mul(v1, v2);
}
inline const double dot(const Vec &v1, const Vec &v2) {
    return Vector3<vecfp_t>::dot(v1, v2);
}
inline const Vec cross(const Vec &v1, const Vec &v2) {
    return Vector3<vecfp_t>::cross(v1, v2);
}
//+++++

}
*/
#endif
