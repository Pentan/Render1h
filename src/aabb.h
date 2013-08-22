#ifndef R1H_AABB_H
#define R1H_AABB_H

#include <cfloat>
#include <algorithm>
#include "vec.h"
#include "ray.h"
#include "intersection.h"

namespace r1h {

struct AABB {
    Vec min;
    Vec max;
    Vec centroid;
    int dataId;
    
    AABB(const double minval=-DBL_MAX, const double maxval=DBL_MAX) : min(maxval), max(minval) {}
    AABB(const Vec minvec, const Vec maxvec) : min(minvec), max(maxvec) {}
    
    void clear(const double minval=-DBL_MAX, const double maxval=DBL_MAX) {
        min = Vec(maxval);
        max = Vec(minval);
    }
    
    Vec getSize() const {
        return max - min;
    }
    
    void expand(const Vec &p) {
        if(p.x_ < min.x_) min.x_ = p.x_;
        if(p.y_ < min.y_) min.y_ = p.y_;
        if(p.z_ < min.z_) min.z_ = p.z_;
        
        if(p.x_ > max.x_) max.x_ = p.x_;
        if(p.y_ > max.y_) max.y_ = p.y_;
        if(p.z_ > max.z_) max.z_ = p.z_;
        
        updateCentroid();
    }
    
    void expand(const AABB &aabb) {
        if(min.x_ > aabb.min.x_) min.x_ = aabb.min.x_;
        if(min.y_ > aabb.min.y_) min.y_ = aabb.min.y_;
        if(min.z_ > aabb.min.z_) min.z_ = aabb.min.z_;
        
        if(max.x_ < aabb.max.x_) max.x_ = aabb.max.x_;
        if(max.y_ < aabb.max.y_) max.y_ = aabb.max.y_;
        if(max.z_ < aabb.max.z_) max.z_ = aabb.max.z_;
        
        updateCentroid();
    }
    
    void updateCentroid() {
        centroid = (min + max) * 0.5;
    }
    
    bool isInside(const Vec &p) const {
        return ((p.x_ > min.x_ && p.y_ > min.y_ && p.z_ > min.z_) &&
                (p.x_ < max.x_ && p.y_ < max.y_ && p.z_ < max.z_) );
    }
    
    bool isIntersect(const Ray &ray, double *outmin=0, int *outaxis=0) const {
        double tmin, tmax;
        double tmpmin, tmpmax;
        int minaxis;
        
        tmin = -DBL_MAX;
        tmax = DBL_MAX;
        
        for(int i = 0; i < 3; i++) {
            
            //std::cout << "check for component [" << i << "]" << std::endl;
            //std::cout << " ray.dir_[" << i << "]:" << ray.dir_.v[i] << std::endl;
            if(fabs(ray.dir_.v[i]) < kEPS) {
                //std::cout << " parallel ray. skip:" << i << " (" << ray.dir_.v[i] << ")" << std::endl;
                continue;
            }
            double vdiv = 1.0 / ray.dir_.v[i];
            //std::cout << " vdiv:" << vdiv << std::endl;
            /*
            if(ray.org_.v[i] > min.v[i] && ray.org_.v[i] < max.v[i]) {
                std::cout << " !!!origin component[" << i << "] " << ray.org_.v[i]  << " is inside a range (" << min.v[i] << "," << max.v[i] << ")" << std::endl;
            }
            */
            if(ray.dir_.v[i] >= 0.0) {
                tmpmin = (min.v[i] - ray.org_.v[i]) * vdiv;
                tmpmax = (max.v[i] - ray.org_.v[i]) * vdiv;
            } else {
                tmpmax = (min.v[i] - ray.org_.v[i]) * vdiv;
                tmpmin = (max.v[i] - ray.org_.v[i]) * vdiv;
            }
            
            // reduction width
            if(tmpmin > tmin) {
                tmin = tmpmin;
                minaxis = i;
            }
            if(tmpmax < tmax) {
                tmax = tmpmax;
            }
            
            //std::cout << " tmp[" << i << "]:" << tmpmin << "," << tmpmax << std::endl;
            //std::cout << " t[" << i << "]:" << tmin << "," << tmax << std::endl;
            
            // not hit
            if(tmax < tmin) {
                //std::cout << "### not hit:" << tmin << "," << tmax << std::endl;
                return false;
            }
            
        }
            
        // behind the ray
        if(tmax < 0.0 && tmin < 0.0) {
            //std::cout << "### intersected but behind." << std::endl;
            return false;
        }
        
        // output
        if(outmin) {
            *outmin = tmin;
        }
        if(outaxis) {
            *outaxis = minaxis;
        }
        
        return true;
    }
};

}
#endif
