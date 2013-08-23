#ifndef R1H_SCENE_H
#define R1H_SCENE_H

#include <vector>

#include "constants.h"
#include "vec.h"
#include "ray.h"
#include "sphere.h"
#include "intersection.h"
#include "material.h"
#include "background.h"
#include "geometry.h"
#include "object.h"
#include "camera.h"
#include "random.h"
#include "render.h"

namespace r1h {

class Scene {
    static const int Depth = 5; // minimum depth
    static const int DepthLimit = 64;
    
public:
    
    Camera *camera;
    std::vector<Object*> objects;
    
    BackgroundMaterial *background;
    
    /////
    Scene() : camera(0), background(0) {}
    
    ~Scene() {
        if(camera) {
            camera->release();
        }
        for(unsigned int i = 0; i < objects.size(); i++) {
            delete objects[i];
        }
    }
    
    int addObject(Object *obj) {
        objects.push_back(obj);
        return objects.size() - 1;
    }
    
    void setCamera(Camera *cam) {
        if(camera) {
            camera->release();
        }
        camera = cam;
        camera->retain();
    }
    
    void setBackgroundMaterial(BackgroundMaterial *bg) {
        if(background) {
            background->release();
        }
        background = bg;
        background->retain();
    }
    
    inline bool intersect_scene(const Ray &ray, Intersection *intersection);
    Color radiance(const Ray &ray, RenderContext *rndrcntx, const int firstdepth);
};

}

#endif
