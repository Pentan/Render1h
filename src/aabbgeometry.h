#ifndef R1H_AABBGEOMETRY_H
#define R1H_AABBGEOMETRY_H

#include "ray.h"
#include "intersection.h"
#include "geometry.h"
#include "aabb.h"

namespace r1h {

struct AABBGeometry : public Geometry {
    
    static const int kTypeID;
    
    AABB aabb;
    
    AABBGeometry(const Vec min, const Vec max): Geometry(kTypeID), aabb(min, max) {}
    
    bool intersect(const Ray &ray, Hitpoint *hitpoint) const {
        double t;
        int axis;
        if(aabb.isIntersect(ray, &t, &axis)) {
            hitpoint->distance_ = t;
            hitpoint->position_ = ray.org_ + ray.dir_ * t;
            hitpoint->materialId = 0;
            
            double d = (ray.dir_.v[axis] > 0.0)? -1.0 : 1.0;
            switch(axis) {
                case 0:
                    hitpoint->normal_ = Vec(d, 0.0, 0.0);
                    break;
                case 1:
                    hitpoint->normal_ = Vec(0.0, d, 0.0);
                    break;
                case 2:
                    hitpoint->normal_ = Vec(0.0, 0.0, d);
                    break;
            }
            
            return true;
        }
        return false;
    }
};

}
#endif
