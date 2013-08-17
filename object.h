#ifndef R1H_OBJECT_H
#define R1H_OBJECT_H

namespace r1h {

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
    
    /////
    //bool intersection(const Ray &ray, Hitpoint *hp) {}
};

}
#endif
