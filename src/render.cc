#include <iostream>

#include "scene.h"
#include "render.h"

using namespace r1h;

int Renderer::render(Scene &sc, const int samples, const int supersamples) {
    // one shot render
    init();
    int ret = resumeRender(sc, samples, supersamples);
    buffer->exportBMP("image.bmp");
    return ret;
}

void Renderer::init() {
    buffer->clear();
    const int height = buffer->getHeight();
    const int seedt = time(0);
    for(int i = 0; i < height; i++) {
        contextArray[i].init(seedt + i);
    }
}

int Renderer::resumeRender(Scene &sc, const int samples, const int supersamples) {
    // image
    const int width = buffer->getWidth();
    const int height = buffer->getHeight();
    
    //std::cout << "render size: " << width << ":" << height << std::endl;
    //std::cout << "samples: " << samples << ", super samples: " << supersamples << " (" << (samples * (supersamples * supersamples)) << " spp)" << std::endl;
    
    Camera *camera = sc.camera;
    
    // rendering (with OpenMP)
//#pragma omp parallel for schedule(dynamic, 1) num_threads(4)
#pragma omp parallel for schedule(dynamic, 1)
    for(int y = 0; y < height; y++) {
        //char msgbuf[64];
        //std::cerr << "Rendering (y = " << y << ")" << (100.0 * y / (height - 1)) << "%" << std::endl;
        std::cout << std::left << "Rendering 1pass ... ";
        std::cout.width(8);
        std::cout << std::right << (100 * y / (height - 1)) << "%   \r";
        std::cout.flush();
        
        RenderContext *rndrcntx = &contextArray[y];
        for(int x = 0; x < width; x++) {
            // super sampling
            for(int sy = 0; sy < supersamples; sy++) {
                for(int sx = 0; sx < supersamples; sx++) {
                    // pixel sampling
                    for(int s = 0; s < samples; s++) {
                        // subsample offsets
                        const double rate = (1.0 / supersamples);
                        const double r1 = (sx + 0.5) * rate;
                        const double r2 = (sy + 0.5) * rate;
                        const double tx = (r1 + x) / width;
                        const double ty = (r2 + y) / height;
                        const Ray camray = camera->getCameraRay(tx, ty);
                        
                        buffer->accumulate(x, y, sc.radiance(camray, rndrcntx, 0));
                    }
                }
            }
        }
    }
    
    std::cout << std::endl;
    
    buffer->incrementSampleCount(samples * supersamples * supersamples);
    
    return 0;
}

int Renderer::resumeRenderWithSSOffset(Scene &sc, const int samples, const int supersamples, const int ssx, const int ssy) {
     
     std::cout << "8/22 20:19 ver. is it latest?" << std::endl;
     // image
    const int width = buffer->getWidth();
    const int height = buffer->getHeight();
    
    Camera *camera = sc.camera;
    
    // rendering (with OpenMP)
//#pragma omp parallel for schedule(dynamic, 1) num_threads(4)
#pragma omp parallel for schedule(dynamic, 1)
    for(int y = 0; y < height; y++) {
        //char msgbuf[64];
        //std::cerr << "Rendering (y = " << y << ")" << (100.0 * y / (height - 1)) << "%" << std::endl;
        std::cout << std::left << "Rendering 1pass ... ";
        std::cout.width(8);
        std::cout << std::right << (100 * y / (height - 1)) << "%   \r";
        std::cout.flush();
        
        RenderContext *rndrcntx = &contextArray[y];
        for(int x = 0; x < width; x++) {
            // super sampling. constant  ssx, xxy
            // pixel sampling
            for(int s = 0; s < samples; s++) {
                // subsample offsets
                const double rate = (1.0 / supersamples);
                const double r1 = (ssx + 0.5) * rate;
                const double r2 = (ssy + 0.5) * rate;
                const double tx = (r1 + x) / width;
                const double ty = (r2 + y) / height;
                const Ray camray = camera->getCameraRay(tx, ty);
                
                buffer->accumulate(x, y, sc.radiance(camray, rndrcntx, 0));
            }
        }
    }
    
    std::cout << std::endl;
    
    buffer->incrementSampleCount(samples);
    
    return 0;
}
