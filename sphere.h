#ifndef R1H_SPHERE_H
#define R1H_SPHERE_H

#include "constants.h"
#include "vec.h"
#include "material.h"
#include "intersection.h"

#include "geometry.h"
#include "material.h"

namespace r1h {

struct Sphere : public Geometry {
    double radius_;
    Vec position_;
    
    //+++++
    Color emission_;
    Color color_;
    ReflectionType reflection_type_;
    //+++++
    
    // edupt style constructor
    Sphere(
        const double radius, const Vec &position,
        const Color &emission, const Color &color,
        const ReflectionType reflection_type) :
        Geometry(),
        radius_(radius), position_(position), emission_(emission), color_(color), reflection_type_(reflection_type)
    {
        EduptMaterial *eduptmat = new EduptMaterial(color_, emission_, reflection_type_);
        addMaterial(eduptmat);
        eduptmat->release();
        
        //std::cout << "Sphere materials.size:" << materials.size() << ", [0]:" << eduptmat << std::endl;
    }
    
    ~Sphere() {
        //printf("Sphere %p destructed\n", this);
    }
    
    bool intersect(const Ray &ray, Hitpoint *hitpoint) const {
        const Vec p_o = position_ - ray.org_;
        const double b = dot(p_o, ray.dir_);
        const double D4 = b * b - dot(p_o, p_o) + radius_ * radius_;
        
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
        hitpoint->normal_ = normalize(hitpoint->position_ - position_);
        hitpoint->material = materials[0];
        return true;
    }
};

}
#endif
