/*
cc -Wall main.cc -lstdc++
g++ -Wall main.cc -lstdc++ -fopenmp
*/

#include <iostream>

#include <cassert>

#include "constants.h"
#include "random.h"
#include "vec.h"
#include "ray.h"
#include "intersection.h"
#include "material.h"
#include "sphere.h"
#include "scene.h"
//#include "bitmap.h"
#include "render.h"
#include "mesh.h"

//+++++
#include <sys/time.h>
static double gettimeofday_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec * 1e-6;
}
//+++++

using namespace r1h;

///////
#include "eduptmaterial.h"

void setupEduptScene(Scene &scene, const double aspect) {
    scene.BackgroundColor = Color(0.0, 0.0, 0.0);
    
    scene.objects.reserve(10);
    
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec( 1e5 + 1.0, 40.8, 81.6),    Color(), Color(0.75, 0.25, 0.25), REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(-1e5 + 99.0, 40.8, 81.6),   Color(), Color(0.25, 0.25, 0.75), REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(50.0, 40.8, 1e5),           Color(), Color(0.75, 0.75, 0.75), REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(50.0, 40.8, 1e5 + 250.0),   Color(), Color(),                 REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(50.0, 1e5, 81.6),           Color(), Color(0.75, 0.75, 0.75), REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(50.0, 1e5 + 81.6, 81.6),    Color(), Color(0.75, 0.75, 0.75), REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(20.0, Vec(65.0, 20.0, 20.0),         Color(), Color(0.25, 0.75, 0.25), REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(16.5, Vec(27.0, 16.5, 47.0),         Color(), Color(0.99, 0.99, 0.99), REFLECTION_TYPE_SPECULAR))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(16.5, Vec(77.0, 16.5, 78.0),         Color(), Color(0.99, 0.99, 0.99), REFLECTION_TYPE_REFRACTION))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(15.0, Vec(50.0, 90.0, 81.6),         Color(36.0, 36.0, 36.0), Color(), REFLECTION_TYPE_DIFFUSE))
    ));
    
    //+++++
    // triangle
    Mesh *mesh = new Mesh(3, 1);
    mesh->addMaterial(RCO::rc0(new EduptMaterial(Color(0.25, 0.75, 0.25), Color(), REFLECTION_TYPE_DIFFUSE)));
    /*
    mesh->addVertexWithAttrs(Vec(8.0, 37.0, 76.5), Vec(0.0, 1.0, 0.0));
    mesh->addVertexWithAttrs(Vec(20.0, 8.0, 100.0), Vec(0.0, 1.0, 0.0));
    mesh->addVertexWithAttrs(Vec(28.0, 22.5, 70.0), Vec(0.0, 1.0, 0.0));
    mesh->addFace(0, 1, 2);
    */
    
    mesh->addVertexWithAttrs(Vec(11.0, 38.0, 81.5), Vec(0.0, 1.0, 0.0));
    mesh->addVertexWithAttrs(Vec(24.0, 16.0, 100.0), Vec(0.0, 1.0, 0.0));
    mesh->addVertexWithAttrs(Vec(38.0, 33.5, 70.0), Vec(0.0, 1.0, 0.0));
    mesh->addVertexWithAttrs(Vec(33.0, 5.5, 73.0), Vec(0.0, 1.0, 0.0));
    mesh->addFace(0, 1, 2);
    mesh->addFace(3, 1, 2);
    mesh->calcSmoothNormals();
    
    //scene.objects.push_back(new Object(mesh));
    mesh->release();
    //+++++
    
    // camera
    scene.camera = RCO::rc0<Camera>(new Camera());
    scene.camera->setLookat(
        Vec(50.0, 52.0, 220.0),
        Vec(50.0, 52.0 - 0.04, 220.0 - 1.0),
        Vec(0.0, 1.0, 0.0)
    );
    scene.camera->setFocal(40.0, 30.0 * aspect);
    scene.camera->setAspectRatio(aspect);
}
///////



// from main.cpp
int main(int argc, char *argv[]) {
    /*
    {
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
    */
    
    std::cout << "Path tracing renderer: edupt copy" << std::endl;
    
    double startT = gettimeofday_sec();
    
    /*
    const int w = 640;
    const int h = 480;
    const int samples = 4;
    const int ss = 2;
    */
    /*
    const int w = 320;
    const int h = 240;
    const int samples = 16;
    const int ss = 2;
    */
    
    const int w = 160;
    const int h = 120;
    const int samples = 64;
    const int ss = 2;
    
    Scene scene;
    
    setupEduptScene(scene, double(w) / h);
    
    Renderer rndr;
    rndr.render(scene, w, h, samples, ss);
    
    std::cout << (gettimeofday_sec() - startT) << " [sec]" << std::endl;
    
    return 0;
}
