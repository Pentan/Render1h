/*
cc -Wall main.cc -lstdc++
g++ -Wall main.cc -lstdc++ -fopenmp
*/

#include <iostream>

#include <cassert>
#include <ctime>
#include <cmath>


//#include "testscene.h"

//+++++
#include <sys/time.h>
static double gettimeofday_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec * 1e-6;
}
//+++++

/*
//////////
// cc -Wall -g test_main.cc -lstdc++
//////////
#include "constants.h"
#include "random.h"
#include "vec.h"
#include "ray.h"
#include "intersection.h"
#include "material.h"
#include "sphere.h"
#include "scene.h"
#include "bitmap.h"
#include "render.h"
#include "mesh.h"
#include "aabb.h"

int main(int argc, char *argv[]) {
    using namespace r1h;
    
    std::cout << "Triangle mesh debug" << std::endl;
    
    
    Mesh *mesh = new Mesh(3, 1);
    mesh->addMaterial(RCO::rc0(new EduptMaterial(Color(0.25, 0.75, 0.25), Color(), REFLECTION_TYPE_DIFFUSE)));
    mesh->addVertexWithAttrs(Vec(0.0, 1.0, 0.0), Vec(0.0, 0.0, 1.0));
    mesh->addVertexWithAttrs(Vec(-1.0, -1.0, 0.0), Vec(0.0, 0.0, 1.0));
    mesh->addVertexWithAttrs(Vec( 1.0, -1.0, 0.0), Vec(0.0, 0.0, 1.0));
    mesh->addFace(0, 1, 2);
    
    const Vec rayorg = Vec(0.0, 0.0, 4.0);
    Hitpoint hp;
    Ray ray(rayorg, Vec(0.0, 0.0, -1.0));        
    if(mesh->intersect(ray, &hp)) {
        printf("(0.0, 0.0, 0.0) hit\n");
    } else {
        printf("(0.0, 0.0, 0.0) out\n");
    }
    
    //return 0;
    
    const int CheckX = 4;
    const int CheckY = 4;
    for(int iy = 0; iy <= CheckY; iy++) {
        double ty = (iy * 2.0 - CheckY) / CheckY;
        for(int ix = 0; ix <= CheckX; ix++) {
            double tx = (ix * 2.0 - CheckX) / CheckX;
            Ray ray(rayorg, normalize(Vec(tx, ty, 0.0) - rayorg));
            
            if(mesh->intersect(ray, &hp)) {
                printf("[%lf, %lf] hit\n", tx, ty);
            } else {
                printf("[%lf, %lf] out\n", tx, ty);
            }
        }
    }
    
    
    mesh->release();
    
    return 0;
}
//////////
*/

//////////
// cc -Wall -g test_main.cc -lstdc++
//////////
#include "constants.h"
#include "random.h"
#include "vec.h"
#include "ray.h"
#include "intersection.h"
#include "aabb.h"

int main(int argc, char *argv[]) {// AABB debug    
    using namespace r1h;
    
    std::cout << "AABB Debug" << std::endl;
    
    AABB aabb;
    
    aabb.expand(Vec(1.0, 1.0, 1.0));
    printf("expand aabb((%lf,%lf,%lf),(%lf,%lf,%lf))\n", aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
    aabb.expand(Vec(1.0, 1.0, -1.0));
    printf("expand aabb((%lf,%lf,%lf),(%lf,%lf,%lf))\n", aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
    aabb.expand(Vec(1.0, -1.0, 0.0));
    printf("expand aabb((%lf,%lf,%lf),(%lf,%lf,%lf))\n", aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
    aabb.expand(Vec(1.0, -1.0, -1.0));
    printf("expand aabb((%lf,%lf,%lf),(%lf,%lf,%lf))\n", aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
    aabb.expand(Vec(-1.0, 1.0, 0.0));
    printf("expand aabb((%lf,%lf,%lf),(%lf,%lf,%lf))\n", aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
    aabb.expand(Vec(-1.0, 1.0, -1.0));
    printf("expand aabb((%lf,%lf,%lf),(%lf,%lf,%lf))\n", aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
    aabb.expand(Vec(-1.0, -1.0, 0.0));
    printf("expand aabb((%lf,%lf,%lf),(%lf,%lf,%lf))\n", aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
    aabb.expand(Vec(-1.0, -1.0, -1.0));
    printf("expand aabb((%lf,%lf,%lf),(%lf,%lf,%lf))\n", aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
    
    Ray ray(Vec(0.0, 0.0, 10.0), Vec(0.0, 0.0, -1.0));
    bool isct;
    
    std::cout << "=== ray hit check ===" << std::endl;
    //isct = aabb.isIntersect(ray);
    //printf("ray((%lf,%lf,%lf),(%lf,%lf,%lf)):%s\n", ray.org_.x_, ray.org_.y_, ray.org_.z_, ray.dir_.x, ray.dir_.y, ray.dir_.z, isct? "Hit":"False");
    
    Random rnd(time(NULL));
    
    const int checkNum = 2;
    for(int y = -checkNum; y <= checkNum; y++) {
        for(int x = -checkNum; x <= checkNum; x++) {
            std::cout << "-----" << std::endl;
            
            double dx = double(x) / checkNum * 1.2;
            double dy = double(y) / checkNum * 1.2;
            Vec target = Vec(dx, dy, 0.0);
            
            /*
            Vec target = Vec(-0.5);
            ray.org_ = Vec(
                (rnd.next01() - 0.5) * 10.0,
                (rnd.next01() - 0.5) * 10.0,
                (rnd.next01() - 0.5) * 10.0
            );
            */
            ray.dir_ = Vec::normalized(target - ray.org_);
            printf("target [%lf,%lf,%lf]\n", target.x_, target.y_, target.z_);
            isct = aabb.isIntersect(ray);
            printf("ray((%lf,%lf,%lf),(%lf,%lf,%lf)):%s\n",
                ray.org_.x_, ray.org_.y_, ray.org_.z_,
                ray.dir_.x, ray.dir_.y, ray.dir_.z,
                isct? "Hit":"False");
        }
    }
    
    return 0;
}
//////////
