#include <iostream>

#include "render.h"
#include "bitmap.h"

using namespace r1h;

int Renderer::render(Scene &sc, const int width, const int height, const int samples, const int supersamples) {
    /*
    // camera parameters
    const Vec camera_pos = Vec(50.0, 52.0, 220.0);
    const Vec camera_dir = normalize(Vec(0.0, -0.04, -1.0));
    const Vec camera_up  = Vec(0.0, 1.0, 0.0);
    
    // screen size in world
    const double screen_width = 30.0 * width / height;
    const double screen_height = 30.0;
    // distance for screen
    const double screen_dist = 40.0;
    // screen vector
    const Vec screen_x = normalize(cross(camera_dir, camera_up)) * screen_width;
    const Vec screen_y = normalize(cross(screen_x, camera_dir)) * screen_height;
    const Vec screen_center = camera_pos + camera_dir * screen_dist;
    */
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
                        /*
                        const Vec screen_pos =
                            screen_center +
                            screen_x * ((r1 + x) / width - 0.5) + 
                            screen_y * ((r2 + y) / height - 0.5);
                        const Vec dir = normalize(screen_pos - camera_pos);
                        //const Ray camray = Ray(camera_pos, dir);
                        */
                        const double tx = (r1 + x) / width;
                        const double ty = (r2 + y) / height;
                        const Ray camray = camera->getCameraRay(tx, ty);
                        
                        /*
                        accumulated_radiance = accumulated_radiance +
                            radiance(Ray(camera_pos, dir), &rnd, 0) / (samples * (supersamples * supersamples));
                        */
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
