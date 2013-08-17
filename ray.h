#ifndef R1H_RAY_H
#define R1H_RAY_H

#include "vec.h"

namespace r1h {

struct Ray {
    Vec org_, dir_;
    Ray(const Vec &org, const Vec &dir): org_(org), dir_(dir) {};
};

}

#endif
