#ifndef R1H_SPHERE_H
#define R1H_SPHERE_H

#include "constants.h"
#include "vec.h"
#include "ray.h"
#include "material.h"
#include "eduptmaterial.h"
#include "intersection.h"
#include "geometry.h"

namespace r1h {

struct Sphere : public Geometry {
    
    static const int kTypeID;
    
    double radius_;
    Vec position_;
    
    // edupt style constructor
    Sphere(
        const double radius, const Vec &position,
        const Color &emission, const Color &color,
        const ReflectionType reflection_type) :
        Geometry(kTypeID), radius_(radius), position_(position)
    {
        EduptMaterial *eduptmat = new EduptMaterial(color, emission, reflection_type);
        addMaterial(eduptmat);
        eduptmat->release();
        //std::cout << "Sphere materials.size:" << materials.size() << ", [0]:" << eduptmat << std::endl;
    }
    
    ~Sphere() {
        //printf("Sphere %p destructed\n", this);
    }
    
    AABB getAABB() const {
        Vec minv = position_ - Vec(radius_);
        Vec maxv = position_ + Vec(radius_);
        return AABB(minv, maxv);
    }
    
    bool intersect(const Ray &ray, Hitpoint *hitpoint) const {
        const Vec p_o = position_ - ray.org_;
        const double b = Vec::dot(p_o, ray.dir_);
        const double D4 = b * b - Vec::dot(p_o, p_o) + radius_ * radius_;
        
        if(D4 < 0.0) return false;
        
        const double sqrt_D4 = sqrt(D4);
        const double t1 = b - sqrt_D4, t2 = b + sqrt_D4;
        
        if(t1 < kEPS && t2 < kEPS) return false;
        if(t1 > kEPS) {
            hitpoint->distance_ = t1;
        } else {
            hitpoint->distance_ = t2;
        }
        
        hitpoint->position_ = ray.org_ + hitpoint->distance_ * ray.dir_;
        hitpoint->normal_ = Vec::normalized(hitpoint->position_ - position_);
        hitpoint->materialId = 0;
        return true;
    }
};

}
#endif
