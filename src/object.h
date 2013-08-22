#ifndef R1H_OBJECT_H
#define R1H_OBJECT_H

#include "geometry.h"

namespace r1h {

class Material;

struct Object {
    Geometry *geometry;
    //std::vector<Material*> materials; // This is in geometry
    // Matrix4 transform;
    
    Object(Geometry *geom=0) : geometry(geom) {
        if(geom) {
            geom->retain();
        }
    }
    
    ~Object() {
        if(geometry) {
            geometry->release();
        }
    }
    
    void setGeometry(Geometry *geom) {
        if(geometry) {
            geometry->release();
        }
        geometry = geom;
        geometry->retain();
    }
    
    Material* getMaterialForId(const int matid) const {
        if(!geometry) {
            return 0;
        }
        if(matid < 0 || matid >= (int)geometry->materials.size()) {
            return 0;
        }
        return geometry->materials[matid];
    }
    
    /////
    //bool intersection(const Ray &ray, Hitpoint *hp) {}
};

}
#endif
