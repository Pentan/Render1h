#ifndef VQM_MATRIX4_H
#define VQM_MATRIX4_H

#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "vector3.h"
#include "vector4.h"

namespace r1h {

template<typename FPType> union Matrix4 {
    struct {
        FPType m00, m01, m02, m03;
        FPType m10, m11, m12, m13;
        FPType m20, m21, m22, m23;
        FPType m30, m31, m32, m33;
    };
    FPType m[16];
    
    /////
    static inline Matrix4 identity() {
        return Matrix4(
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
        );
    }
    
    // utilities
    static inline void sprint(char *buf, const Matrix4 &m, const bool isbr) {
        if(isbr) {
            sprintf(buf, "|%.4f %.4f %.4f %.4f|\n|%.4f %.4f %.4f %.4f|\n|%.4f %.4f %.4f %.4f|\n|%.4f %.4f %.4f %.4f|",
                m.m00, m.m10, m.m20, m.m30,
                m.m01, m.m11, m.m21, m.m31,
                m.m02, m.m12, m.m22, m.m32,
                m.m03, m.m13, m.m23, m.m33
            );
        } else {
            sprintf(buf, "|(%.4f %.4f %.4f %.4f)(%.4f %.4f %.4f %.4f)(%.4f %.4f %.4f %.4f)(%.4f %.4f %.4f %.4f)|",
                m.m00, m.m10, m.m20, m.m30,
                m.m01, m.m11, m.m21, m.m31,
                m.m02, m.m12, m.m22, m.m32,
                m.m03, m.m13, m.m23, m.m33
            );
        }
    }
    
    // constructors
    Matrix4():
        m00(0.0), m01(0.0), m02(0.0), m03(0.0),
        m10(0.0), m11(0.0), m12(0.0), m13(0.0),
        m20(0.0), m21(0.0), m22(0.0), m23(0.0),
        m30(0.0), m31(0.0), m32(0.0), m33(0.0)
    {}
    Matrix4(
        const FPType im00, const FPType im01, const FPType im02, const FPType im03,
        const FPType im10, const FPType im11, const FPType im12, const FPType im13,
        const FPType im20, const FPType im21, const FPType im22, const FPType im23,
        const FPType im30, const FPType im31, const FPType im32, const FPType im33):
        m00(im00), m01(im01), m02(im02), m03(im03),
        m10(im10), m11(im11), m12(im12), m13(im13),
        m20(im20), m21(im21), m22(im22), m23(im23),
        m30(im30), m31(im31), m32(im32), m33(im33)
    {}
    Matrix4(const FPType im[16]):
        m00(im[0]), m01(im[1]), m02(im[2]), m03(im[3]),
        m10(im[4]), m11(im[5]), m12(im[6]), m13(im[7]),
        m20(im[8]), m21(im[9]), m22(im[10]), m23(im[11]),
        m30(im[12]), m31(im[13]), m32(im[14]), m33(im[15])
    {}
    
    // self operation
    inline void set(
        const FPType nm00, const FPType nm01, const FPType nm02, const FPType nm03,
        const FPType nm10, const FPType nm11, const FPType nm12, const FPType nm13,
        const FPType nm20, const FPType nm21, const FPType nm22, const FPType nm23,
        const FPType nm30, const FPType nm31, const FPType nm32, const FPType nm33)
    {
        m00 = nm00;
        m01 = nm01;
        m02 = nm02;
        m03 = nm03;
        m10 = nm10;
        m11 = nm11;
        m12 = nm12;
        m13 = nm13;
        m20 = nm20;
        m21 = nm21;
        m22 = nm22;
        m23 = nm23;
        m30 = nm30;
        m31 = nm31;
        m32 = nm32;
        m33 = nm33;
    }
    inline void set(const FPType nm[16]) {
        memcpy(m, nm, sizeof(FPType) * 16);
    }
    inline void setColumns(const Vector4<FPType> &c0, const Vector4<FPType> &c1, const Vector4<FPType> &c2, const Vector4<FPType> &c3) {
        memcpy(&m[0],  c0.v, sizeof(FPType) * 4);
        memcpy(&m[4],  c1.v, sizeof(FPType) * 4);
        memcpy(&m[8],  c2.v, sizeof(FPType) * 4);
        memcpy(&m[12], c3.v, sizeof(FPType) * 4);
    }
    inline void setRows(const Vector4<FPType> &r0, const Vector4<FPType> &r1, const Vector4<FPType> &r2, const Vector4<FPType> &r3) {
        m00 = r0.x;
        m01 = r1.x;
        m02 = r2.x;
        m03 = r3.x;
        m10 = r0.y;
        m11 = r1.y;
        m12 = r2.y;
        m13 = r3.y;
        m20 = r0.z;
        m21 = r1.z;
        m22 = r2.z;
        m23 = r3.z;
        m30 = r0.w;
        m31 = r1.w;
        m32 = r2.w;
        m33 = r3.w;
    }
    
    inline void setTranslation(const FPType tx, const FPType ty, const FPType tz) {
        *this = Matrix4::identity();
        m30 = tx;
        m31 = ty;
        m32 = tz;
    }
    inline void setRotation(const FPType rad, const FPType ax, const FPType ay, const FPType az) {
        FPType sn = sin(rad);
        FPType cs = cos(rad);
        m00 = ax * ax * (1.0 - cs) + cs;
        m01 = ax * ay * (1.0 - cs) + sn * az;
        m02 = ax * az * (1.0 - cs) + sn * -ay;
        m03 = 0.0;
        
        m10 = ax * ay * (1.0 - cs) + sn * -az;
        m11 = ay * ay * (1.0 - cs) + cs;
        m12 = ay * az * (1.0 - cs) + sn * ax;
        m13 = 0.0;
        
        m20 = ax * az * (1.0 - cs) + sn * ay;
        m21 = ay * az * (1.0 - cs) + sn * -ax;
        m22 = az * az * (1.0 - cs) + cs;
        m23 = 0.0;
        
        m30 = 0.0;
        m31 = 0.0;
        m32 = 0.0;
        m33 = 1.0;
    }
    inline void setScale(const FPType sx, const FPType sy, const FPType sz) {
        *this = Matrix4::identity();
        m00 = sx;
        m11 = sy;
        m22 = sz;
    }
    
    inline Vector4<FPType> getColumn(const int col) const {
        FPType *tmpm = &m[col * 4];
        return Vector4<FPType>(tmpm[0], tmpm[1], tmpm[2], tmpm[3]);
    }
    inline void setColumn(const int col, const Vector4<FPType> &v) {
        memcpy(&m[col * 4], v.v, sizeof(FPType) * 4);
    }
    inline Vector4<FPType> getRow(const int row) const {
        return Vector4<FPType>(m[row], m[row + 4], m[row + 8], m[row + 12]);
    }
    inline void setRow(const int row, const Vector4<FPType> &v) {
        m[row] = v.x;
        m[row + 4] = v.y;
        m[row + 8] = v.z;
        m[row + 12] = v.w;
    }
    
    inline bool invert() {
        int i, j, k;
        
        Vector4<FPType> mrows[4];
        Vector4<FPType> invrows[4] = {
            Vector4<FPType>(1.0, 0.0, 0.0, 0.0),
            Vector4<FPType>(0.0, 1.0, 0.0, 0.0),
            Vector4<FPType>(0.0, 0.0, 1.0, 0.0),
            Vector4<FPType>(0.0, 0.0, 0.0, 1.0)
        };
        Vector4<FPType> *mrowptr[4], *invrowptr[4];
        
        // init
        for(i = 0; i < 4; i++) {
            mrows[i] = this->getRow(i);
            
            mrowptr[i] = &mrows[i];
            invrowptr[i] = &invrows[i];
        }
        
        // calc
        for(i = 0; i < 4; i++) {
            //choose pivot
            int maxrow = i;
            FPType maxabs = fabs(mrowptr[i]->v[i]);
            for(j = i + 1; j < 4; j++) {
                if(maxabs < fabs(mrowptr[j]->v[i])) {
                    maxabs = fabs(mrowptr[j]->v[i]);
                    maxrow = j;
                }
            }
            // swap
            if(maxrow != i) {
                Vector4<FPType> *swapv = mrowptr[i];
                mrowptr[i] = mrowptr[maxrow];
                mrowptr[maxrow] = swapv;
                
                swapv = invrowptr[i];
                invrowptr[i] = invrowptr[maxrow];
                invrowptr[maxrow] = swapv;
            }
            // eliminate
            float pvtval = mrowptr[i]->v[i];
            if(fabs(pvtval) < VEC_EPS) {
                return false;
            }
            // pivot row
            for(k = 0; k < 4; k++) {
                mrowptr[i]->v[k] /= pvtval;
                invrowptr[i]->v[k] /= pvtval;
            }
            // left rows
            for(j = 0; j < 4; j++) {
                if(j == i) continue;
                FPType tmpval = mrowptr[j]->v[i];
                for(k = 0; k < 4; k++) {
                    mrowptr[j]->v[k] -= mrowptr[i]->v[k] * tmpval;
                    invrowptr[j]->v[k] -= invrowptr[i]->v[k] * tmpval;
                }
            }
        }
        
        m00 = invrowptr[0]->x;
        m01 = invrowptr[1]->x;
        m02 = invrowptr[2]->x;
        m03 = invrowptr[3]->x;
        
        m10 = invrowptr[0]->y;
        m11 = invrowptr[1]->y;
        m12 = invrowptr[2]->y;
        m13 = invrowptr[3]->y;
        
        m20 = invrowptr[0]->z;
        m21 = invrowptr[1]->z;
        m22 = invrowptr[2]->z;
        m23 = invrowptr[3]->z;
        
        m30 = invrowptr[0]->w;
        m31 = invrowptr[1]->w;
        m32 = invrowptr[2]->w;
        m33 = invrowptr[3]->w;
        
        return true;
    }
    inline void transpose() {
        std::swap(m01, m10);
        std::swap(m02, m20);
        std::swap(m03, m30);
        std::swap(m12, m21);
        std::swap(m13, m31);
        std::swap(m23, m32);
    }
    inline bool invTrans() {
        bool ret = invert();
        transpose();
        return ret;
    }
    inline void translate(const FPType tx,const FPType ty, const FPType tz) {
        m30 = m00 * tx + m10 * ty + m20 * tz + m30;
        m31 = m01 * tx + m11 * ty + m21 * tz + m31;
        m32 = m02 * tx + m12 * ty + m22 * tz + m32;
    }
    inline void translate(const Vector3<FPType> &transv) {
        translate(transv.x, transv.y, transv.z);
    }
    inline void rotate(const FPType rad, const FPType ax, const FPType ay, const FPType az) {
        *this = *this * Matrix4::makeRotation(rad, ax, ay, az);
    }
    inline void rotate(const FPType rad, const Vector3<FPType> &axisv) {
        *this = *this * Matrix4::makeRotation(rad, axisv.x, axisv.y, axisv.z);
    }
    inline void scale(const FPType sx, const FPType sy, const FPType sz) {
        m00 *= sx;
        m01 *= sx;
        m02 *= sx;
        m03 *= sx;
        
        m10 *= sy;
        m11 *= sy;
        m12 *= sy;
        m13 *= sy;
        
        m20 *= sz;
        m21 *= sz;
        m22 *= sz;
        m23 *= sz;
    }

    inline void scale(const Vector3<FPType> &scalev) {
        m00 *= scalev.x;
        m01 *= scalev.y;
        m02 *= scalev.z;
        
        m10 *= scalev.x;
        m11 *= scalev.y;
        m12 *= scalev.z;
        
        m20 *= scalev.x;
        m21 *= scalev.y;
        m22 *= scalev.z;
    }

    // 1 matrix operations
    static inline Matrix4 makeTranslation(const FPType tx, const FPType ty, const FPType tz) {
        Matrix4 retm;
        retm.setTranslation(tx, ty, tz);
        return retm;
    }
    static inline Matrix4 makeRotation(const FPType rad, const FPType ax, const FPType ay, const FPType az) {
        Matrix4 retm;
        retm.setRotation(rad, ax, ay, az);
        return retm;
    }
    static inline Matrix4 makeScale(const FPType sx, const FPType sy, const FPType sz) {
        Matrix4 retm;
        retm.setScale(sx, sy, sz);
        return retm;
    }

    static inline Matrix4 makeOrtho(const FPType left, const FPType right, const FPType bottom, const FPType top, const FPType nearz, const FPType farz) {
        Matrix4 retm;
        retm.m00 = 2.0 / (right - left);
        retm.m01 = 0.0;
        retm.m02 = 0.0;
        retm.m03 = 0.0;
        
        retm.m10 = 0.0;
        retm.m11 = 2.0 / (top - bottom);
        retm.m12 = 0.0;
        retm.m13 = 0.0;
        
        retm.m20 = 0.0;
        retm.m21 = 0.0;
        retm.m22 = -2.0 / (farz - nearz);
        retm.m23 = 0.0;
        
        retm.m30 = -(right + left) / (right - left);
        retm.m31 = -(top + bottom) / (top - bottom);
        retm.m32 = -(farz + nearz) / (farz - nearz);
        retm.m33 = 1.0;
        return retm;
    }
    static inline Matrix4 makeFrustum(const FPType left, const FPType right, const FPType bottom, const FPType top, const FPType nearz, const FPType farz) {
        Matrix4 retm;
        retm.m00 = 2.0 * nearz / (right - left);
        retm.m01 = 0.0;
        retm.m02 = 0.0;
        retm.m03 = 0.0;
        
        retm.m10 = 0.0;
        retm.m11 = 2.0 * nearz / (top - bottom);
        retm.m12 = 0.0;
        retm.m13 = 0.0;
        
        retm.m20 = (right + left) / (right - left);
        retm.m21 = (top + bottom) / (top - bottom);
        retm.m22 = -(farz + nearz) / (farz - nearz);
        retm.m23 = -1.0;
        
        retm.m30 = 0.0;
        retm.m31 = 0.0;
        retm.m32 = -2.0 * farz * nearz / (farz - nearz);
        retm.m33 = 0.0;
        return retm;
    }
    static inline Matrix4 makePerspective(const FPType yrad, const FPType aspect, const FPType nearz, const FPType farz) {
        FPType top = tan(yrad * 0.5f) * nearz;
        FPType right = top * aspect;
        return Matrix4::makeFrustum(-right, right, -top, top, nearz, farz);
    }
    static inline Matrix4 makeLookAt(const FPType eyex, const FPType eyey, const FPType eyez, const FPType lookx, const FPType looky, const FPType lookz, const FPType upx, const FPType upy, const FPType upz) {
        Matrix4 retm;
        
        Vector3<FPType> upv = Vector3<FPType>::normalized(Vector3<FPType>(upx, upy, upz));
        Vector3<FPType> eyev = Vector3<FPType>::normalized(Vector3<FPType>(eyex - lookx, eyey - looky, eyez - lookz));
        Vector3<FPType> sidev = Vector3<FPType>::normalized(Vector3<FPType>::cross(upv, eyev));
        upv = Vector3<FPType>::normalized(Vector3<FPType>::cross(eyev, sidev));
        
        retm.m00 = sidev.x;
        retm.m01 = upv.x;
        retm.m02 = eyev.x;
        retm.m03 = 0.0;
        
        retm.m10 = sidev.y;
        retm.m11 = upv.y;
        retm.m12 = eyev.y;
        retm.m13 = 0.0;
        
        retm.m20 = sidev.z;
        retm.m21 = upv.z;
        retm.m22 = eyev.z;
        retm.m23 = 0.0;
        
        retm.m30 = 0.0;
        retm.m31 = 0.0;
        retm.m32 = 0.0;
        retm.m33 = 1.0;
        
        return Matrix4::translated(retm, -eyex, -eyey, -eyez);
    }
    
    static inline Matrix4 inverted(const Matrix4 &m, bool *isinv) {
        Matrix4 tmpm = m;
        bool invres = tmpm.invert();
        if(isinv != NULL) {
            *isinv = invres;
        }
        return tmpm;
    }
    static inline Matrix4 transposed(const Matrix4 &m) {
        Matrix4 tmpm = m;
        tmpm.transpose();
        return tmpm;
    }
    static inline Matrix4 invTransed(const Matrix4 &m, bool *isinv) {
        Matrix4 tmpm = m;
        bool invres = tmpm.invTrans();
        if(isinv != NULL) {
            *isinv = invres;
        }
        return tmpm;
    }
    static inline Matrix4 translated(const Matrix4 m, const FPType tx, const FPType ty, const FPType tz) {
        Matrix4 tmpm = m;
        tmpm.translate(tx, ty, tz);
        return tmpm;
    }
    static inline Matrix4 translated(const Matrix4 m, const Vector3<FPType> &transv) {
        Matrix4 tmpm = m;
        tmpm.translate(transv);
        return tmpm;
    }
    static inline Matrix4 rotated(const Matrix4 m, const FPType rad, const FPType ax, const FPType ay, const FPType az) {
        Matrix4 tmpm = m;
        tmpm.rotate(rad, ax, ay, az);
        return tmpm;
    }
    static inline Matrix4 rotated(const Matrix4 m, const FPType rad, const Vector3<FPType> axisv) {
        Matrix4 tmpm = m;
        tmpm.rotate(rad, axisv);
        return tmpm;
    }
    static inline Matrix4 scaled(const Matrix4 m, const FPType sx, const FPType sy, const FPType sz) {
        Matrix4 tmpm = m;
        tmpm.scale(sx, sy, sz);
        return tmpm;
    }
    static inline Matrix4 scaled(const Matrix4 m, const Vector3<FPType> scalev) {
        Matrix4 tmpm = m;
        tmpm.scale(scalev);
        return tmpm;
    }

    // vs vectors
    static inline Vector3<FPType> mulV3(const Matrix4 ml, const Vector3<FPType> vr) {
        Vector3<FPType> retv;
        retv.x = ml.m00 * vr.x + ml.m10 * vr.y + ml.m20 * vr.z;
        retv.y = ml.m01 * vr.x + ml.m11 * vr.y + ml.m21 * vr.z;
        retv.z = ml.m02 * vr.x + ml.m12 * vr.y + ml.m22 * vr.z;
        return retv;
    }
    static inline Vector3<FPType> mulV3WithTrans(const Matrix4 ml, const Vector3<FPType> vr) {
        Vector3<FPType> retv;
        retv.x = ml.m00 * vr.x + ml.m10 * vr.y + ml.m20 * vr.z + ml.m30;
        retv.y = ml.m01 * vr.x + ml.m11 * vr.y + ml.m21 * vr.z + ml.m31;
        retv.z = ml.m02 * vr.x + ml.m12 * vr.y + ml.m22 * vr.z + ml.m32;
        return retv;
    }
    
    static inline Vector4<FPType> mulV4(const Matrix4 &ml, const Vector4<FPType> &vr) {
        Vector4<FPType> retv;
        retv.x = ml.m00 * vr.x + ml.m10 * vr.y + ml.m20 * vr.z + ml.m30 * vr.w;
        retv.y = ml.m01 * vr.x + ml.m11 * vr.y + ml.m21 * vr.z + ml.m31 * vr.w;
        retv.z = ml.m02 * vr.x + ml.m12 * vr.y + ml.m22 * vr.z + ml.m32 * vr.w;
        retv.w = ml.m03 * vr.x + ml.m13 * vr.y + ml.m23 * vr.z + ml.m33 * vr.w;
        return retv;
    }

    static inline Vector3<FPType> mulAndProjectV3(const Matrix4 ml, const Vector3<FPType> vr) {
        Vector4<FPType> v4 = Vector4<FPType>(vr, 1.0);
        v4 = Matrix4::mulV4(ml, v4) * (1.0 / v4.w);
        return v4.getXYZ();
    }
    
    // operaters
    inline Matrix4 operator+(const Matrix4 &mr) {
        Matrix4<FPType> retm;
        retm.m00 = m00 + mr.m00;
        retm.m01 = m01 + mr.m01;
        retm.m02 = m02 + mr.m02;
        retm.m03 = m03 + mr.m03;
        
        retm.m10 = m10 + mr.m10;
        retm.m11 = m11 + mr.m11;
        retm.m12 = m12 + mr.m12;
        retm.m13 = m13 + mr.m13;
        
        retm.m20 = m20 + mr.m20;
        retm.m21 = m21 + mr.m21;
        retm.m22 = m22 + mr.m22;
        retm.m23 = m23 + mr.m23;
        
        retm.m30 = m30 + mr.m30;
        retm.m31 = m31 + mr.m31;
        retm.m32 = m32 + mr.m32;
        retm.m33 = m33 + mr.m33;
        
        return retm;
    }
    inline Matrix4 operator-(const Matrix4 &mr) {
        Matrix4 retm;
        retm.m00 = m00 - mr.m00;
        retm.m01 = m01 - mr.m01;
        retm.m02 = m02 - mr.m02;
        retm.m03 = m03 - mr.m03;
        
        retm.m10 = m10 - mr.m10;
        retm.m11 = m11 - mr.m11;
        retm.m12 = m12 - mr.m12;
        retm.m13 = m13 - mr.m13;
        
        retm.m20 = m20 - mr.m20;
        retm.m21 = m21 - mr.m21;
        retm.m22 = m22 - mr.m22;
        retm.m23 = m23 - mr.m23;
        
        retm.m30 = m30 - mr.m30;
        retm.m31 = m31 - mr.m31;
        retm.m32 = m32 - mr.m32;
        retm.m33 = m33 - mr.m33;
        
        return retm;
    }
    inline Matrix4 operator*(const Matrix4 &mr) {
        Matrix4 retm;
        retm.m00 = m00 * mr.m00 + m10 * mr.m01 + m20 * mr.m02 + m30 * mr.m03;
        retm.m01 = m01 * mr.m00 + m11 * mr.m01 + m21 * mr.m02 + m31 * mr.m03;
        retm.m02 = m02 * mr.m00 + m12 * mr.m01 + m22 * mr.m02 + m32 * mr.m03;
        retm.m03 = m03 * mr.m00 + m13 * mr.m01 + m23 * mr.m02 + m33 * mr.m03;
        retm.m10 = m00 * mr.m10 + m10 * mr.m11 + m20 * mr.m12 + m30 * mr.m13;
        retm.m11 = m01 * mr.m10 + m11 * mr.m11 + m21 * mr.m12 + m31 * mr.m13;
        retm.m12 = m02 * mr.m10 + m12 * mr.m11 + m22 * mr.m12 + m32 * mr.m13;
        retm.m13 = m03 * mr.m10 + m13 * mr.m11 + m23 * mr.m12 + m33 * mr.m13;
        retm.m20 = m00 * mr.m20 + m10 * mr.m21 + m20 * mr.m22 + m30 * mr.m23;
        retm.m21 = m01 * mr.m20 + m11 * mr.m21 + m21 * mr.m22 + m31 * mr.m23;
        retm.m22 = m02 * mr.m20 + m12 * mr.m21 + m22 * mr.m22 + m32 * mr.m23;
        retm.m23 = m03 * mr.m20 + m13 * mr.m21 + m23 * mr.m22 + m33 * mr.m23;
        retm.m30 = m00 * mr.m30 + m10 * mr.m31 + m20 * mr.m32 + m30 * mr.m33;
        retm.m31 = m01 * mr.m30 + m11 * mr.m31 + m21 * mr.m32 + m31 * mr.m33;
        retm.m32 = m02 * mr.m30 + m12 * mr.m31 + m22 * mr.m32 + m32 * mr.m33;
        retm.m33 = m03 * mr.m30 + m13 * mr.m31 + m23 * mr.m32 + m33 * mr.m33;
        return retm;
    }
    inline Matrix4 operator*(const FPType s) {
        Matrix4 retm = *this;
        for(int i = 0; i < 16; i++) {
            retm.v[i] *= s;
        }
        return retm;
    }
};

// VS scalar
template<typename FPType> inline Matrix4<FPType> operator*(const FPType s, Matrix4<FPType> &m) {
    return m * s;
}

// VS scalar
template<typename FPType> inline Matrix4<FPType> operator/(const FPType s, Matrix4<FPType> &m) {
    return m / s;
}

}
#endif
