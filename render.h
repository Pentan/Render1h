#ifndef R1H_RENDER_H
#define R1H_RENDER_H

#include "constants.h"
#include "vec.h"
#include "scene.h"
#include "material.h"

namespace r1h {

class Renderer {
    Color *image;
public:
    Renderer() : image(0) {}
    ~Renderer() {
        if(image) delete [] image;
    }
    int render(Scene &sc, const int width, const int height, const int samples, const int supersamples);
};

}

#endif
