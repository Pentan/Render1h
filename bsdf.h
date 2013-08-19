#ifndef R1H_BSDF_H
#define R1H_BSDF_H

#include <vector>
#include "constants.h"
#include "ray.h"
#include "intersection.h"
#include "random.h"
#include "vec.h"

namespace r1h {

class BSDF {
public:
    virtual ~BSDF() {}
    virtual void getNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<TraceInfo> &outvecs) = 0;
};

}
#endif
