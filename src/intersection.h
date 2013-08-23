#ifndef R1H_INTERSECTION_H
#define R1H_INTERSECTION_H

#include <vector>
#include "constants.h"
#include "vec.h"
//#include "material.h"

namespace r1h {

struct Hitpoint {
    double distance_;
    Vec normal_;
    Vec position_;
    
    int materialId;
    int faceId;
    Vec varyingWeight;
    
    
    struct {
        Vec position;
        Vec normal;
        Vec tangent;
    } world, local;
    
    std::vector<Vec> attributes;
    
    //Material *material;
    
    Hitpoint() : distance_(kINF), normal_(), position_() {}
    
    Vec orientingNormal(const Ray &ray) const {
        return (Vec::dot(normal_, ray.dir_) < 0.0)? normal_ : (normal_ * -1.0);
    }
};

struct Intersection {
    Hitpoint hitpoint_;
    int object_id_;
    
    Intersection() : object_id_(-1) {}
};

}

#endif
