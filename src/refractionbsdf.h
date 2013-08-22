#ifndef R1H_REFRACTIONBSDF_H
#define R1H_REFRACTIONBSDF_H

#include "bsdf.h"

namespace r1h {

const double kIor = 1.5; // ior of REFLECTION_TYPE_REFRACTION

class RefractionBSDF : public BSDF {
public:
    ~RefractionBSDF() {}
    void getNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<TraceInfo> &outvecs) {
        const Ray reflection_ray = Ray(hp.position_, ray.dir_ - hp.normal_ * 2.0 * Vec::dot(hp.normal_, ray.dir_));
        const Vec orienting_normal = hp.orientingNormal(ray);
        const bool into = Vec::dot(hp.normal_, orienting_normal) > 0.0;
        
        // Snell's law
        const double nc = 1.0;
        const double nt = kIor;
        const double nnt = into ? (nc / nt) : (nt / nc);
        const double ddn = Vec::dot(ray.dir_, orienting_normal);
        const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);
        
        // full reflection
        if(cos2t < 0.0) {
            TraceInfo nxttrc;
            nxttrc.ray = reflection_ray;
            nxttrc.weight = Color(1.0, 1.0, 1.0);
            outvecs.push_back(nxttrc);
            /*
            incoming_radiance = radiance(reflection_ray, rnd, depth + 1);
            weight = now_color / russian_roulette_probability;
            */
            return;
        }
        
        // refraction
        const Ray refraction_ray = Ray(
            hp.position_,
            Vec::normalized(ray.dir_ * nnt - hp.normal_ * (into ? 1.0 : -1.0) * (ddn * nnt + sqrt(cos2t)))
            );
        
        // Schlick' Fresnel aproximation
        const double a = nt - nc, b = nt + nc;
        const double R0 = (a * a) / (b * b);
        
        const double c = 1.0 - (into ? -ddn : Vec::dot(refraction_ray.dir_, -1.0 * orienting_normal));
        const double Re = R0 + (1.0 - R0) * pow(c, 5.0);
        const double nnt2 = pow(into ? (nc / nt) : (nt / nc), 2.0);
        const double Tr = (1.0 - Re) * nnt2;
        
        // In deep step case, choose reflect or refract by russian roulette.
        const double probability = 0.25 + 0.5 * Re;
        if(depth > 2) {
            TraceInfo nxttrc;
            if(rnd->next01() < probability) { // reflection
                nxttrc.ray = reflection_ray;
                nxttrc.weight = Color(Re / (1.0 - probability));
                /*
                incoming_radiance = radiance(reflection_ray, rnd, depth + 1) * Re;
                weight = now_color / ((1.0 - probability) * russian_roulette_probability);
                */
            } else { // refraction
                nxttrc.ray = refraction_ray;
                nxttrc.weight = Color(Tr / (1.0 - probability));
                /*
                incoming_radiance = radiance(refraction_ray, rnd, depth + 1) * Tr;
                weight = now_color / ((1.0 - probability) * russian_roulette_probability);
                */
            }
            outvecs.push_back(nxttrc);
        } else { // shallow case, calculate both
            TraceInfo retrc;
            retrc.ray = reflection_ray;
            retrc.weight = Color(Re);
            outvecs.push_back(retrc);
            
            TraceInfo trtrc;
            trtrc.ray = refraction_ray;
            trtrc.weight = Color(Tr);
            outvecs.push_back(trtrc);
            /*
            incoming_radiance =
                radiance(reflection_ray, rnd, depth + 1) * Re +
                radiance(refraction_ray, rnd, depth + 1) * Tr;
            weight = now_color / russian_roulette_probability;
            */
        }
    }
};

}
#endif
