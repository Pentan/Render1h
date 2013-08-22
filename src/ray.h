#ifndef R1H_RAY_H
#define R1H_RAY_H

#include "constants.h"
#include "vec.h"
#include "color.h"

namespace r1h {

struct Ray {
    Vec org_, dir_;
    
    Ray() : org_(0.0, 0.0, 0.0), dir_(0.0, 0.0, -1.0) {};
    Ray(const Vec &org, const Vec &dir): org_(org), dir_(dir) {};
    
    void smallOffset(const Vec &n) {
        org_ += n * ((Vec::dot(dir_, n) > 0.0)? kEPS : -kEPS);
    }
};

struct TraceInfo {
    Ray ray;
    Color weight;
    
    TraceInfo(): ray(), weight() {};
    TraceInfo(const Ray &r, const Color &w): ray(r), weight(w) {};
};

}
#endif
