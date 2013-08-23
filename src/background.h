#ifndef R1H_BACKGROUND_H
#define R1H_BACKGROUND_H

namespace r1h {

struct BackgroundMaterial : public Material {
    
    BackgroundMaterial() {};
    ~BackgroundMaterial() {};
    
    Color albedo(const Object &obj, const Hitpoint &hp) const { return Vec(0.0); };
    Color emission(const Object &obj, const Hitpoint &hp)  const { return Vec(1.0); };
    void calcNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<TraceInfo> &outvecs) {};
    
    virtual Color backgroundColor(const Ray &ray) { return Color(0.8); }
};

}
#endif
