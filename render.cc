#include <iostream>

#include "render.h"
#include "bitmap.h"

using namespace r1h;

int Renderer::render(Scene &sc, const int width, const int height, const int samples, const int supersamples) {
    // image
    Color *image = new Color[width * height];
    
    std::cout << "render size: " << width << ":" << height << std::endl;
    std::cout << "samples: " << samples << ", super samples: " << supersamples << " (" << (samples * (supersamples * supersamples)) << " spp)" << std::endl;
    
    Camera *camera = sc.camera;
    
    // rendering (with OpenMP)
#pragma omp parallel for schedule(dynamic, 1) num_threads(4)
    for(int y = 0; y < height; y++) {
        std::cerr << "Rendering (y = " << y << ")" << (100.0 * y / (height - 1)) << "%" << std::endl;
        
        Random rnd(y + 1);
        for(int x = 0; x < width; x++) {
            const int image_index = (height - y - 1) * width + x;
            // super sampling
            for(int sy = 0; sy < supersamples; sy++) {
                for(int sx = 0; sx < supersamples; sx++) {
                    // pixel sampling
                    Color accumulated_radiance = Color();
                    for(int s = 0; s < samples; s++) {
                        // subsample offsets
                        const double rate = (1.0 / supersamples);
                        const double r1 = (sx + 0.5) * rate;
                        const double r2 = (sy + 0.5) * rate;
                        const double tx = (r1 + x) / width;
                        const double ty = (r2 + y) / height;
                        const Ray camray = camera->getCameraRay(tx, ty);
                        
                        accumulated_radiance = accumulated_radiance + sc.radiance(camray, &rnd, 0);
                    }
                    image[image_index] = image[image_index] + accumulated_radiance / samples;
                }
            }
            image[image_index] = image[image_index] / (supersamples * supersamples);
        }
    }
    
    // save
    save_bmp_file(std::string("image.bmp"), image, width, height);
    
    return 0;
}
