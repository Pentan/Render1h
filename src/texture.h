#ifndef R1H_TEXTURE_H
#define R1H_TEXTURE_H

#include <vector>
#include "color.h"
#include "vec.h"
#include "rco.h"

namespace r1h {

// base class
class Texture : RCO {
    bool isUseTrans;
    Mat4 transform;
    
public:    
    void setIsUseTransform(const bool isuse) { isUseTrans = isuse; }
    void setTransform(const Mat4 m){ transform = m; }
    
    Vec applyTransform(const Vec &p) const;
    
    Texture(): isUseTrans(true) {}
    virtual ~Texture() {};
    virtual Color sample(const Vec &p) const = 0;
};

// noise(bw)
class NoiseTexture : public Texture {
public:
    enum IpoType {
        Cell,
        Smooth
    };
    
private:
    int *noise;
    IpoType ipotype;
    
    double fade(const double t) const;
    double lerp(const double t, const double a, const double b) const;
    double grad(const int hash, const double x, const double y, const double z) const;
    
public:
    
    NoiseTexture(const IpoType ipo=Smooth, const int seed=1);
    ~NoiseTexture();
    
    void setSeed(const int seed);
    void setIpoType(const IpoType ipo){ ipotype = ipo; };
    
    Color sample(const Vec &p) const;
};

// voronoi(bw)
class VoronoiTexture : public Texture {
    NoiseTexture *noiseTexs[3];
    double jitter;
public:
    VoronoiTexture(const double j=0.9, const int seed=1);
    ~VoronoiTexture();
    
    void setJitter(const double j) { jitter = j; }
    
    Color sample(const Vec &p) const;
};

// fractal noise (bw)
class FractalNoiseTexture : public Texture {
public:
    enum NoiseType {
        CellNoise,
        SmoothNoise,
        Voronoi
    };
    
private:
    std::vector<Texture*>textures;
    int depth;
    int noiseSeed;
    double voroJitter;
    NoiseType noiseType;
    
public:
    FractalNoiseTexture(const NoiseType noisetype, const int d=4, const int seed=1);
    ~FractalNoiseTexture();
    
    void setDepth(const int d);
    void setJitter(const double j); // Voronoi only
    
    Color sample(const Vec &p) const;
};

}
#endif
