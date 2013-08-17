#ifndef R1H_INTERSECTION_H
#define R1H_INTERSECTION_H

#include "constants.h"
#include "vec.h"
#include "material.h"

namespace r1h {

struct Hitpoint {
    double distance_;
    Vec normal_;
    Vec position_;
    Material *material;
    
    Hitpoint() : distance_(kINF), normal_(), position_() {}
};

struct Intersection {
    Hitpoint hitpoint_;
    int object_id_;
    
    Intersection() : object_id_(-1) {}
};

}

#endif
