#ifndef R1H_MATERIAL_H
#define R1H_MATERIAL_H

#include "rco.h"

namespace r1h {

const double kIor = 1.5; // ior of REFLECTION_TYPE_REFRACTION
typedef Vec Color;

enum ReflectionType {
    REFLECTION_TYPE_DIFFUSE,
    REFLECTION_TYPE_SPECULAR,
    REFLECTION_TYPE_REFRACTION
};

// don't use delete! use release()
class Material : public RCO {
public:
    struct Coord {
        Vec world;
        Vec local;
        Vec uv;
    };
    
    //virtual ~Material() {}
    virtual Color albedo(const Coord &co) const = 0; //{ return Color(0.0, 0.0, 0.0); };
    virtual Color emission(const Coord &co) const = 0; //{ return Color(1.0, 0.0, 0.0); };
    virtual ReflectionType reflectionType(const Coord &co) const = 0; //{ return REFLECTION_TYPE_DIFFUSE; };
};

///////
class EduptMaterial : public Material {
    Color color_;
    Color emission_;
    ReflectionType reflection_type_;
    
public:
    EduptMaterial(const Color &col, const Color &emit, const ReflectionType reft):
        color_(col), emission_(emit), reflection_type_(reft)
    {}
    ~EduptMaterial() {
        //printf("EduptMaterial %p destructed\n", this);
    }
    
    Color albedo(const Coord &co) const {
        //std::cout << "EduptMaterial albedo" << std::endl;
        return color_;
    }
    Color emission(const Coord &co) const {
        //std::cout << "EduptMaterial emission" << std::endl;
        return emission_;
    }
    ReflectionType reflectionType(const Coord &co) const {
        //std::cout << "EduptMaterial reflectionType" << std::endl;
        return reflection_type_;
    }
};


}

#endif
