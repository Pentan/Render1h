#ifndef R1H_SPECULARBSDF_H
#define R1H_SPECULARBSDF_H

#include "bsdf.h"

namespace r1h {

class SpecularBSDF : public BSDF {
public:
    ~SpecularBSDF() {}
    void getNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<TraceInfo> &outvecs) {
        TraceInfo nxttrc;
        nxttrc.ray = Ray(hp.position_, ray.dir_ - hp.normal_ * 2.0 * Vec::dot(hp.normal_, ray.dir_));
        nxttrc.weight = Color(1.0);
        outvecs.push_back(nxttrc);
        /*
        incoming_radiance = radiance(, rnd, depth + 1);
        weight = now_color / russian_roulette_probability;
        */
    }
};

}
#endif
