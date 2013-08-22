#ifndef R1H_RENDER_H
#define R1H_RENDER_H

#include <vector>
#include "constants.h"
#include "vec.h"
#include "renderbuffer.h"
//#include "scene.h"
#include "material.h"

namespace r1h {

class Scene;

struct RenderContext  {
    Random random;
    std::vector<TraceInfo> traceVec1;
    std::vector<TraceInfo> traceVec2;
    std::vector<TraceInfo> workVec;
    
    RenderContext(): random(0) {}
    
    void init(const int rndseed) {
        random.setSeed(rndseed);
        traceVec1.reserve(128);
        traceVec2.reserve(128);
        workVec.reserve(32);
    }
};

class Renderer {
    RenderContext *contextArray;
    
public:
    RenderBuffer *buffer;
    
    Renderer(const int width, const int height) : buffer(0) {
        buffer = new RenderBuffer(width, height);
        contextArray = new RenderContext[height];
    }
    ~Renderer() {
        if(buffer) { delete buffer; }
        if(contextArray) { delete [] contextArray; }
    }
    
    // one call rendering. export "image.bmp"
    int render(Scene &sc, const int samples, const int supersamples);
    
    // step by step rendering.
    void init();
    int resumeRender(Scene &sc, const int samples, const int supersamples);
    void exportBMP(const char *bmppath) {
        buffer->exportBMP(bmppath);
    }
    
    // special! for too long time to rendering...
    int resumeRenderWithSSOffset(Scene &sc, const int samples, const int supersamples, const int ssx, const int ssy);
};

}

#endif
