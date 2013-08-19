#ifndef R1H_DIFFUSEBSDF_H
#define R1H_DIFFUSEBSDF_H

#include "bsdf.h"

namespace r1h {

class DiffuseBSDF : public BSDF {
public:
    ~DiffuseBSDF() {}
    void getNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<TraceInfo> &outvecs) {
        Vec w, u, v;
        w = hp.orientingNormal(ray);
        
        if(fabs(w.x_) > kEPS) {
            u = normalize(cross(Vec(0.0, 1.0, 0.0), w));
        } else {
            u = normalize(cross(Vec(1.0, 0.0, 0.0), w));
        }
        v = cross(w, u);
        
        // importance sampling using cosine.
        const double r1 = 2.0 * kPI * rnd->next01();
        const double r2 = rnd->next01();
        const double r2s = sqrt(r2);
        Vec dir = normalize((
            u * cos(r1) * r2s +
            v * sin(r1) * r2s +
            w * sqrt(1.0 - r2)
        ));
        
        TraceInfo nexttrc;
        nexttrc.ray = Ray(hp.position_, dir);
        nexttrc.weight = Color(1.0, 1.0, 1.0);
        
        outvecs.push_back(nexttrc);
        /*
        incoming_radiance = radiance(, rnd, depth + 1);
        weight = now_color / russian_roulette_probability;
        */
    }
};

}
#endif
