/*
cc -Wall main.cc -lstdc++
g++ -Wall main.cc -lstdc++ -fopenmp
*/

#include <iostream>

#include <cassert>
#include <ctime>
#include <cmath>

#include "constants.h"
#include "random.h"
#include "vec.h"
#include "ray.h"
#include "intersection.h"
#include "material.h"
#include "sphere.h"
#include "scene.h"
#include "render.h"
#include "mesh.h"
#include "aabb.h"

#include "testscene.h"

//+++++
#include <sys/time.h>
static double gettimeofday_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec * 1e-6;
}
//+++++

using namespace r1h;

// from main.cpp
int main(int argc, char *argv[]) {
    std::cout << "Path tracing renderer forked from edupt" << std::endl;
    
    double startT = gettimeofday_sec();
    
    const struct {
        int w, h;
        int s, ss;
    } renderSpects[] = {
        {640, 480,   4, 2}, //0
        {320, 240,  16, 2}, //1
        {160, 120,  32, 2}, //2
        {160, 120,  64, 2}, //3
        {160, 120,   8, 2}, //4
        {160, 120, 256, 2}, //5
        {320, 240,   4, 1}, //6
        {320, 240,   8, 2}, //7
        {640, 480,  16, 2}, //8
        
        {  0,   0,   0, 0}  //terminate
    };
    const int useSpec = 4; //
    
    const int w = renderSpects[useSpec].w;
    const int h = renderSpects[useSpec].h;
    const int samples = renderSpects[useSpec].s;
    const int ss = renderSpects[useSpec].ss;
    
    Scene scene;
    
    //+++++
    // scene setup
    //setupEduptScene(scene, double(w) / h);
    //setupTestObjScene(scene, double(w) / h);
    //setupCornelBoxObjScene(scene, double(w) / h);
    //setupAABBTestScene(scene, double(w) / h);
    setupManyTrisObjScene(scene, double(w) / h);
    //return 0;
    //+++++
    
    Renderer rndr(w, h);
    rndr.render(scene, samples, ss);
    
    std::cout << (gettimeofday_sec() - startT) << " [sec]" << std::endl;
    
    return 0;
}
