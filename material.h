#ifndef R1H_MATERIAL_H
#define R1H_MATERIAL_H

#include "rco.h"
#include "color.h"
#include "random.h"
#include "intersection.h"

namespace r1h {

enum ReflectionType {
    REFLECTION_TYPE_DIFFUSE,
    REFLECTION_TYPE_SPECULAR,
    REFLECTION_TYPE_REFRACTION
};

// don't use delete! use release()
class Material : public RCO {
public:
    //virtual ~Material() {}
    virtual Color albedo(const Hitpoint &hp) const = 0; //{ return Color(0.0, 0.0, 0.0); };
    virtual Color emission(const Hitpoint &hp) const = 0; //{ return Color(1.0, 0.0, 0.0); };
    
    virtual void calcNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<TraceInfo> &outvecs) = 0;
    
    //+++++
    virtual ReflectionType reflectionType(const Hitpoint &hp) const = 0; //{ return REFLECTION_TYPE_DIFFUSE; };
};

}

#endif
