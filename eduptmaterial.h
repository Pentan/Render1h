#ifndef R1H_EDUPTMATERIAL_H
#define R1H_EDUPTMATERIAL_H

#include "rco.h"
#include "material.h"
#include "bsdf.h"
#include "diffusebsdf.h"
#include "specularbsdf.h"
#include "refractionbsdf.h"

namespace r1h {

/*
enum ReflectionType {
    REFLECTION_TYPE_DIFFUSE,
    REFLECTION_TYPE_SPECULAR,
    REFLECTION_TYPE_REFRACTION
};
*/

class EduptMaterial : public Material {
    Color color_;
    Color emission_;
    ReflectionType reflection_type_;
    
    BSDF *bsdf;
    
public:
    EduptMaterial(const Color &col, const Color &emit, const ReflectionType reft):
        color_(col), emission_(emit), reflection_type_(reft)
    {
        switch(reflection_type_) {
            case REFLECTION_TYPE_DIFFUSE:
                bsdf = new DiffuseBSDF();
                break;
            case REFLECTION_TYPE_SPECULAR:
                bsdf = new SpecularBSDF();
                break;
            case REFLECTION_TYPE_REFRACTION:
                bsdf = new RefractionBSDF();
                break;
        }
    }
    ~EduptMaterial() {
        //printf("EduptMaterial %p destructed\n", this);
        delete bsdf;
    }
    
    Color albedo(const Hitpoint &hp) const {
        //std::cout << "EduptMaterial albedo" << std::endl;
        return color_;
    }
    Color emission(const Hitpoint &hp) const {
        //std::cout << "EduptMaterial emission" << std::endl;
        return emission_;
    }
    void calcNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<TraceInfo> &outvecs) {
        bsdf->getNextRays(ray, hp, depth, rnd, outvecs);
    }
    
    ReflectionType reflectionType(const Hitpoint &hp) const {
        //std::cout << "EduptMaterial reflectionType" << std::endl;
        return reflection_type_;
    }
    
    
};


}

#endif
