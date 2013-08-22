
#include <cstdio>
#include <cstring>
#include <cmath>
#include "matrix4.h"

using namespace r1h;

const Matrix4 Matrix4::Identity = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
};

union Matrix4 {
    struct {
        FPType m00, m01, m02, m03;
        FPType m10, m11, m12, m13;
        FPType m20, m21, m22, m23;
        FPType m30, m31, m32, m33;
    };
    FPType m[16];

    // utilities
    static inline void Matrix4::sprint(char *buf, const Matrix4 m, const bool isbr) {
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
    inline void setColumns(const Vector4 c0, const Vector4 c1, const Vector4 c2, const Vector4 c3) {
        memcpy(&m[0],  c0.v, sizeof(FPType) * 4);
        memcpy(&m[4],  c1.v, sizeof(FPType) * 4);
        memcpy(&m[8],  c2.v, sizeof(FPType) * 4);
        memcpy(&m[12], c3.v, sizeof(FPType) * 4);
    }
    inline void setRows(const Vector4 r0, const Vector4 r1, const Vector4 r2, const Vector4 r3) {
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
    /*
    inline void set(const Quaternion q) {
        m00 = 1.0 - 2.0f * (q.y * q.y + q.z * q.z);
        m01 = 2.0 * (q.x * q.y + q.z * q.w);
        m02 = 2.0 * (q.z * q.x - q.w * q.y);
        m03 = 0.0;
        
        m10 = 2.0 * (q.x * q.y - q.z * q.w);
        m11 = 1.0 - 2.0f * (q.z * q.z + q.x * q.x);
        m12 = 2.0 * (q.y * q.z + q.w * q.x);
        m13 = 0.0;
        
        m20 = 2.0 * (q.z * q.x + q.w * q.y);
        m21 = 2.0 * (q.y * q.z - q.x * q.w);
        m22 = 1.0 - 2.0f * (q.x * q.x + q.y * q.y);
        m23 = 0.0;
        
        m30 = 0.0;
        m31 = 0.0;
        m32 = 0.0;
        m33 = 1.0;
    }
    */

    inline void setTranslation(const FPType tx, const FPType ty, const FPType tz) {
        *this = Matrix4Identity;
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
        *this = Matrix4Identity;
        m00 = sx;
        m11 = sy;
        m22 = sz;
    }
    
    inline Vector4 getColumn(const int col) const {
        return Vector4::make(&m[col * 4]);
    }
    inline void setColumn(const int col, const Vector4 v) {
        memcpy(&m[col * 4], v.v, sizeof(FPType) * 4);
    }
    inline Vector4 getRow(const int row) const {
        return Vector4::make(m[row], m[row + 4], m[row + 8], m[row + 12]);
    }
    inline void setRow(const int row, const Vector4 v) {
        m[row] = v.x;
        m[row + 4] = v.y;
        m[row + 8] = v.z;
        m[row + 12] = v.w;
    }
    inline Matrix3 getMatrix3() const {
        return Matrix3::make(m00, m01, m02, m10, m11, m12, m20, m21, m22);
    }

    inline bool invert() {
        int i, j, k;
        
        Vector4 mrows[4], invrows[4];
        Vector4 *mrowptr[4], *invrowptr[4];
        
        // init
        for(i = 0; i < 4; i++) {
            mrows[i] = this->getRow(i);
            invrows[i] = Matrix4Identity.getRow(i);
            
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
                Vector4 *swapv = mrowptr[i];
                mrowptr[i] = mrowptr[maxrow];
                mrowptr[maxrow] = swapv;
                
                swapv = invrowptr[i];
                invrowptr[i] = invrowptr[maxrow];
                invrowptr[maxrow] = swapv;
            }
            // eliminate
            float pvtval = mrowptr[i]->v[i];
            if(fabs(pvtval) < VQM_EPS) {
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
        swapFP(&m01, &m10);
        swapFP(&m02, &m20);
        swapFP(&m03, &m30);
        swapFP(&m12, &m21);
        swapFP(&m13, &m31);
        swapFP(&m23, &m32);
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
    inline void translate(const Vector3 transv) {
        translate(transv.x, transv.y, transv.z);
    }
    inline void rotate(const FPType rad, const FPType ax, const FPType ay, const FPType az) {
        *this = Matrix4::mul(*this, Matrix4::makeRotation(rad, ax, ay, az));
    }
    inline void rotate(const FPType rad, const Vector3 axisv) {
        *this = Matrix4::mul(*this, Matrix4::makeRotation(rad, axisv.x, axisv.y, axisv.z));
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

    inline void scale(const Vector3 scalev) {
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
    static inline Matrix4 make(
        const FPType m00, const FPType m01, const FPType m02, const FPType m03,
        const FPType m10, const FPType m11, const FPType m12, const FPType m13,
        const FPType m20, const FPType m21, const FPType m22, const FPType m23,
        const FPType m30, const FPType m31, const FPType m32, const FPType m33)
    {
        Matrix4 retm;
        retm.set(
            m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33);
        return retm;
    }
    static inline Matrix4 make(const FPType m[16]) {
        Matrix4 retm;
        retm.set(m);
        return retm;
    }
    static inline Matrix4 makeWithColumns(const Vector4 c0, const Vector4 c1, const Vector4 c2, const Vector4 c3) {
        Matrix4 retm;
        retm.setColumns(c0, c1, c2, c3);
        return retm;
    }
    static inline Matrix4 makeWithRows(const Vector4 r0, const Vector4 r1, const Vector4 r2, const Vector4 r3) {
        Matrix4 retm;
        retm.setRows(r0, r1, r2, r3);
        return retm;
    }
    /*
    static inline Matrix4 make(VQMQuaternion q) {
        Matrix4 retm;
        retm.set(q);
        return retm;
    }
    */
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
        
        Vector3 upv = Vector3::normalized(Vector3::make(upx, upy, upz));
        Vector3 eyev = Vector3::normalized(Vector3::make(eyex - lookx, eyey - looky, eyez - lookz));
        Vector3 sidev = Vector3::normalized(Vector3::cross(upv, eyev));
        upv = Vector3::normalized(Vector3::cross(eyev, sidev));
        
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

    static inline Matrix4 inverted(Matrix4 m, bool *isinv) {
        bool invres = m.invert();
        if(isinv != NULL) {
            *isinv = invres;
        }
        return m;
    }
    static inline Matrix4 transposed(Matrix4 m) {
        m.transpose();
        return m;
    }
    static inline Matrix4 invTransed(Matrix4 m, bool *isinv) {
        bool invres = m.invTrans();
        if(isinv != NULL) {
            *isinv = invres;
        }
        return m;
    }
    static inline Matrix4 translated(Matrix4 m, const FPType tx, const FPType ty, const FPType tz) {
        m.translate(tx, ty, tz);
        return m;
    }
    static inline Matrix4 translated(Matrix4 m, const Vector3 transv) {
        m.translate(transv);
        return m;
    }
    static inline Matrix4 rotated(Matrix4 m, const FPType rad, const FPType ax, const FPType ay, const FPType az) {
        m.rotate(rad, ax, ay, az);
        return m;
    }
    static inline Matrix4 rotated(Matrix4 m, const FPType rad, const Vector3 axisv) {
        m.rotate(rad, axisv);
        return m;
    }
    static inline Matrix4 scaled(Matrix4 m, const FPType sx, const FPType sy, const FPType sz) {
        m.scale(sx, sy, sz);
        return m;
    }
    static inline Matrix4 scaled(Matrix4 m, const Vector3 scalev) {
        m.scale(scalev);
        return m;
    }

    // 2 matrix operations
    static inline Matrix4 add(const Matrix4 ml, const Matrix4 mr) {
        Matrix4 retm;
        retm.m00 = ml.m00 + mr.m00;
        retm.m01 = ml.m01 + mr.m01;
        retm.m02 = ml.m02 + mr.m02;
        retm.m03 = ml.m03 + mr.m03;
        
        retm.m10 = ml.m10 + mr.m10;
        retm.m11 = ml.m11 + mr.m11;
        retm.m12 = ml.m12 + mr.m12;
        retm.m13 = ml.m13 + mr.m13;
        
        retm.m20 = ml.m20 + mr.m20;
        retm.m21 = ml.m21 + mr.m21;
        retm.m22 = ml.m22 + mr.m22;
        retm.m23 = ml.m23 + mr.m23;
        
        retm.m30 = ml.m30 + mr.m30;
        retm.m31 = ml.m31 + mr.m31;
        retm.m32 = ml.m32 + mr.m32;
        retm.m33 = ml.m33 + mr.m33;
        
        return retm;
    }
    static inline Matrix4 sub(const Matrix4 ml, const Matrix4 mr) {
        Matrix4 retm;
        retm.m00 = ml.m00 - mr.m00;
        retm.m01 = ml.m01 - mr.m01;
        retm.m02 = ml.m02 - mr.m02;
        retm.m03 = ml.m03 - mr.m03;
        
        retm.m10 = ml.m10 - mr.m10;
        retm.m11 = ml.m11 - mr.m11;
        retm.m12 = ml.m12 - mr.m12;
        retm.m13 = ml.m13 - mr.m13;
        
        retm.m20 = ml.m20 - mr.m20;
        retm.m21 = ml.m21 - mr.m21;
        retm.m22 = ml.m22 - mr.m22;
        retm.m23 = ml.m23 - mr.m23;
        
        retm.m30 = ml.m30 - mr.m30;
        retm.m31 = ml.m31 - mr.m31;
        retm.m32 = ml.m32 - mr.m32;
        retm.m33 = ml.m33 - mr.m33;
        
        return retm;
    }
    static inline Matrix4 mul(const Matrix4 ml, const Matrix4 mr) {
        Matrix4 retm;
        retm.m00 = ml.m00 * mr.m00 + ml.m10 * mr.m01 + ml.m20 * mr.m02 + ml.m30 * mr.m03;
        retm.m01 = ml.m01 * mr.m00 + ml.m11 * mr.m01 + ml.m21 * mr.m02 + ml.m31 * mr.m03;
        retm.m02 = ml.m02 * mr.m00 + ml.m12 * mr.m01 + ml.m22 * mr.m02 + ml.m32 * mr.m03;
        retm.m03 = ml.m03 * mr.m00 + ml.m13 * mr.m01 + ml.m23 * mr.m02 + ml.m33 * mr.m03;
        retm.m10 = ml.m00 * mr.m10 + ml.m10 * mr.m11 + ml.m20 * mr.m12 + ml.m30 * mr.m13;
        retm.m11 = ml.m01 * mr.m10 + ml.m11 * mr.m11 + ml.m21 * mr.m12 + ml.m31 * mr.m13;
        retm.m12 = ml.m02 * mr.m10 + ml.m12 * mr.m11 + ml.m22 * mr.m12 + ml.m32 * mr.m13;
        retm.m13 = ml.m03 * mr.m10 + ml.m13 * mr.m11 + ml.m23 * mr.m12 + ml.m33 * mr.m13;
        retm.m20 = ml.m00 * mr.m20 + ml.m10 * mr.m21 + ml.m20 * mr.m22 + ml.m30 * mr.m23;
        retm.m21 = ml.m01 * mr.m20 + ml.m11 * mr.m21 + ml.m21 * mr.m22 + ml.m31 * mr.m23;
        retm.m22 = ml.m02 * mr.m20 + ml.m12 * mr.m21 + ml.m22 * mr.m22 + ml.m32 * mr.m23;
        retm.m23 = ml.m03 * mr.m20 + ml.m13 * mr.m21 + ml.m23 * mr.m22 + ml.m33 * mr.m23;
        retm.m30 = ml.m00 * mr.m30 + ml.m10 * mr.m31 + ml.m20 * mr.m32 + ml.m30 * mr.m33;
        retm.m31 = ml.m01 * mr.m30 + ml.m11 * mr.m31 + ml.m21 * mr.m32 + ml.m31 * mr.m33;
        retm.m32 = ml.m02 * mr.m30 + ml.m12 * mr.m31 + ml.m22 * mr.m32 + ml.m32 * mr.m33;
        retm.m33 = ml.m03 * mr.m30 + ml.m13 * mr.m31 + ml.m23 * mr.m32 + ml.m33 * mr.m33;
        return retm;
    }

    static inline Vector3 mulV3(const Matrix4 ml, const Vector3 vr) {
        Vector3 retv;
        retv.x = ml.m00 * vr.x + ml.m10 * vr.y + ml.m20 * vr.z;
        retv.y = ml.m01 * vr.x + ml.m11 * vr.y + ml.m21 * vr.z;
        retv.z = ml.m02 * vr.x + ml.m12 * vr.y + ml.m22 * vr.z;
        return retv;
    }
    static inline void mulV3Array(const Matrix4 ml, Vector3 *vrarray, const size_t vcount) {
        size_t i;
        for(i = 0; i < vcount; i++) {
            vrarray[i] = Matrix4::mulV3(ml, vrarray[i]);
        }
    }
    static inline Vector3 mulV3WithTrans(const Matrix4 ml, const Vector3 vr) {
        Vector3 retv;
        retv.x = ml.m00 * vr.x + ml.m10 * vr.y + ml.m20 * vr.z + ml.m30;
        retv.y = ml.m01 * vr.x + ml.m11 * vr.y + ml.m21 * vr.z + ml.m31;
        retv.z = ml.m02 * vr.x + ml.m12 * vr.y + ml.m22 * vr.z + ml.m32;
        return retv;
    }
    static inline void mulV3ArrayWithTrans(const Matrix4 ml, Vector3 *vrarray, const size_t vcount) {
        for(size_t i = 0; i < vcount; i++) {
            vrarray[i] = Matrix4::mulV3WithTrans(ml, vrarray[i]);
        }
    }
    static inline Vector4 mulV4(const Matrix4 ml, const Vector4 vr) {
        Vector4 retv;
        retv.x = ml.m00 * vr.x + ml.m10 * vr.y + ml.m20 * vr.z + ml.m30 * vr.w;
        retv.y = ml.m01 * vr.x + ml.m11 * vr.y + ml.m21 * vr.z + ml.m31 * vr.w;
        retv.z = ml.m02 * vr.x + ml.m12 * vr.y + ml.m22 * vr.z + ml.m32 * vr.w;
        retv.w = ml.m03 * vr.x + ml.m13 * vr.y + ml.m23 * vr.z + ml.m33 * vr.w;
        return retv;
    }
    static inline void mulV4Array(const Matrix4 ml, Vector4 *vrarray, const size_t vcount) {
        for(size_t i = 0; i < vcount; i++) {
            vrarray[i] = Matrix4::mulV4(ml, vrarray[i]);
        }
    }

    static inline Vector3 mulAndProjectV3(const Matrix4 ml, const Vector3 vr) {
        Vector4 v4 = Vector4::make(vr, 1.0);
        v4 = Vector4::mulScalar(Matrix4::mulV4(ml, v4), 1.0 / v4.w);
        return v4.getXYZ();
    }
    static inline void mulAndProjectV3Array(const Matrix4 ml, Vector3 *vrarray, const size_t vcount) {
        for(size_t i = 0; i < vcount; i++) {
            vrarray[i] = Matrix4::mulAndProjectV3(ml, vrarray[i]);
        }
    }
};

}
#endif
