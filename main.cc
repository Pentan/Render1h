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

//+++++
#include <sys/time.h>
static double gettimeofday_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec * 1e-6;
}
//+++++

// from main.cpp
int main(int argc, char *argv[]) {
    using namespace r1h;
    
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
    const int samples = 32;
    const int ss = 2;
    
    Scene scene;
    scene.setupEduptScene(double(w) / h);
    
    Renderer rndr;
    rndr.render(scene, 640, 480, 4, 2);
    //rndr.render(scene, 320, 240, 16, 2);
    //rndr.render(scene, w, h, samples, ss);
    
    std::cout << (gettimeofday_sec() - startT) << " [sec]" << std::endl;
    
    return 0;
}
