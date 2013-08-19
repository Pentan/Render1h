#ifndef R1H_CAMERA_H
#define R1H_CAMERA_H

#include <cmath>
#include "vec.h"
#include "ray.h"
#include "rco.h"

namespace r1h {

class Camera : public RCO {
public:
    Vec pos;
    Vec dir;
    Vec up;
    Vec side;
    
    // screensize of dir.length is 1.0
    //double focalWidth;
    //double focalHeight;
    
    double screenLeft;  // screen width of dir.length is 1.0
    double aspectRatio;
    
    Camera() :
        pos(0.0, 0.0, 0.0),
        dir(0.0, 0.0, -1.0),
        up(0.0, 1.0, 0.0),
        screenLeft(1.0),
        aspectRatio(1.778)
    {}
    
    void setLookat(const Vec &eye, const Vec &look, const Vec &nup) {
        pos = eye;
        dir = normalize(look - eye);
        up = nup;
        side = normalize(cross(dir, up));
        up = normalize(cross(dir, side));
    }
    
    // width / height
    void setAspectRatio(const double aspect) {
        aspectRatio = aspect;
    }
    
    void setFocal(const double focalmm, const double sensorwidth=36.0) {
        screenLeft = sensorwidth / focalmm;
    }
    
    void setFieldOfView(const double vdegree) {
        screenLeft = tan(vdegree * M_PI / 180.0 * 0.5) * 2.0;
    }
    
    Ray getCameraRay(const double tx, const double ty) {
        Vec left = side * (screenLeft * (tx - 0.5));
        Vec top = up * (screenLeft / aspectRatio * (ty - 0.5) * -1.0);
        return Ray(pos, normalize(dir + left + top));
    }
};

}

#endif
