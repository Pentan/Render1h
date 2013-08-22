#ifndef R1H_MATERIAL_H
#define R1H_MATERIAL_H

#include <iostream>
#include "rco.h"
#include "color.h"
#include "random.h"
#include "intersection.h"
//#include "object.h"

namespace r1h {

struct Object;

// don't use delete! use release()
class Material : public RCO {
public:
    virtual ~Material() {}
    /*
    ~Material() {
        std::cout << "Material destructed" << std::endl;
    }
    */
    virtual Color albedo(const Object &obj, const Hitpoint &hp) const = 0;
    virtual Color emission(const Object &obj, const Hitpoint &hp) const = 0;
    virtual void calcNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<TraceInfo> &outvecs) = 0;
};

}

#endif
