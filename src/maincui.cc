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

#include <sys/time.h>
static double gettimeofday_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec * 1e-6;
}

using namespace r1h;

// from main.cpp
int main(int argc, char *argv[]) {
    std::cout << "Path tracing renderer forked from edupt" << std::endl;
    
    double startSec = gettimeofday_sec();
    
    const int w = 640;
    const int h = 480;
    const int samples = 2;
    const int ss = 4;
    
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
    rndr.init();
    
    const time_t ExportInterval = 60;
    int exportCount = 0;
    char namebuf[128];
    
    const time_t RenderStopTime = 10 * 60; //55 * 60; //[sec]
    time_t startTime = time(0);
    time_t elapsedTime = 0;
    
    int loopCount = 1;
    do {
        std::cout << "rendering... pass[" << loopCount << "]" << std::endl;
        rndr.resumeRender(scene, samples, ss);
        
        //+++++
        std::cout << "sleep 5 sec" << std::endl;
        usleep(5 * 1000000);
        //+++++
        
        elapsedTime = time(0) - startTime;
        
        if(elapsedTime > ((exportCount + 1)  * ExportInterval)) {
            sprintf(namebuf, "image_%04d.bmp", exportCount);
            rndr.exportBMP(namebuf);
            ++exportCount;
            std::cout << "progress export:" << namebuf << std::endl;
        }
        
        ++loopCount;
        
    } while(elapsedTime < RenderStopTime);
    
    int totalsamples = rndr.buffer->getCurrentSampleCount();
    std::cout << "total samples:" << (totalsamples / (ss * ss)) << std::endl;
    
    rndr.exportBMP("image_final.bmp");
    
    std::cout << (gettimeofday_sec() - startSec) << " [sec]" << std::endl;
    
    return 0;
}
