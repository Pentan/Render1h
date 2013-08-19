#ifndef R1H_SCENE_H
#define R1H_SCENE_H

#include <vector>

#include "constants.h"
#include "vec.h"
#include "ray.h"
#include "sphere.h"
#include "intersection.h"
#include "material.h"
#include "geometry.h"
#include "object.h"
#include "camera.h"
#include "random.h"

namespace r1h {

class Scene {
    static const int Depth = 5; // minimum depth
    static const int DepthLimit = 64;
    
public:
    
    Camera *camera;
    std::vector<Object*> objects;
    
    Color BackgroundColor;
    
    
    Scene() : camera(0), BackgroundColor(0.0, 0.0, 0.0) {}
    
    ~Scene() {
        if(camera) {
            camera->release();
        }
        for(unsigned int i = 0; i < objects.size(); i++) {
            delete objects[i];
        }
    }
    
    void setCamera(Camera *cam) {
        if(camera) {
            camera->release();
        }
        camera = cam;
        camera->retain();
    }
    
    inline bool intersect_scene(const Ray &ray, Intersection *intersection);
    Color radiance(const Ray &ray, Random *rnd, const int firstdepth);
};

}

#endif
