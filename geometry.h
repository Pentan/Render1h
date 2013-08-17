#ifndef R1H_GEOMETRY_H
#define R1H_GEOMETRY_H

#include <vector>
#include "rco.h"
#include "material.h"

namespace r1h {

struct Geometry : public RCO {
    
    std::vector<Material*> materials;
    
    Geometry() : RCO() {}
    virtual ~Geometry() {
        for(unsigned int i = 0; i < materials.size(); i++) {
            materials[i]->release();
        }
        //std::cout << "Geometry destructed" << std::endl;
    }
    
    
    void addMaterial(Material *mat) {
        materials.push_back(mat);
        mat->retain();
    }
    virtual bool intersect(const Ray &ray, Hitpoint *hitpoint) const { return false; }
};

}
#endif
