#ifndef R1H_GEOMETRY_H
#define R1H_GEOMETRY_H

#include <vector>
#include "rco.h"
#include "material.h"
#include "ray.h"
#include "intersection.h"

namespace r1h {

class Material;

struct Geometry : public RCO {
    int typeID;
    std::vector<Material*> materials;
    
    Geometry(int tid=0) : RCO(), typeID(tid) {}
    virtual ~Geometry() {
        for(unsigned int i = 0; i < materials.size(); i++) {
            materials[i]->release();
        }
        //std::cout << "Geometry destructed" << std::endl;
    }
    
    
    int addMaterial(Material *mat) {
        materials.push_back(mat);
        mat->retain();
        return materials.size() - 1;
    }
    virtual bool intersect(const Ray &ray, Hitpoint *hitpoint) const { return false; }
};

}
#endif
