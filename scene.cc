

#include "scene.h"

using namespace r1h;

void Scene::setupEduptScene(const double aspect) {
    BackgroundColor = Color(0.0, 0.0, 0.0);
    
    objects.reserve(10);
    
    objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec( 1e5 + 1.0, 40.8, 81.6),    Color(), Color(0.75, 0.25, 0.25), REFLECTION_TYPE_DIFFUSE))
    ));
    objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(-1e5 + 99.0, 40.8, 81.6),   Color(), Color(0.25, 0.25, 0.75), REFLECTION_TYPE_DIFFUSE))
    ));
    objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(50.0, 40.8, 1e5),           Color(), Color(0.75, 0.75, 0.75), REFLECTION_TYPE_DIFFUSE))
    ));
    objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(50.0, 40.8, 1e5 + 250.0),   Color(), Color(),                 REFLECTION_TYPE_DIFFUSE))
    ));
    objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(50.0, 1e5, 81.6),           Color(), Color(0.75, 0.75, 0.75), REFLECTION_TYPE_DIFFUSE))
    ));
    objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(50.0, 1e5 + 81.6, 81.6),    Color(), Color(0.75, 0.75, 0.75), REFLECTION_TYPE_DIFFUSE))
    ));
    objects.push_back(new Object(
        RCO::rc0(new Sphere(20.0, Vec(65.0, 20.0, 20.0),         Color(), Color(0.25, 0.75, 0.25), REFLECTION_TYPE_DIFFUSE))
    ));
    objects.push_back(new Object(
        RCO::rc0(new Sphere(16.5, Vec(27.0, 16.5, 47.0),         Color(), Color(0.99, 0.99, 0.99), REFLECTION_TYPE_SPECULAR))
    ));
    objects.push_back(new Object(
        RCO::rc0(new Sphere(16.5, Vec(77.0, 16.5, 78.0),         Color(), Color(0.99, 0.99, 0.99), REFLECTION_TYPE_REFRACTION))
    ));
    objects.push_back(new Object(
        RCO::rc0(new Sphere(15.0, Vec(50.0, 90.0, 81.6),         Color(36.0, 36.0, 36.0), Color(), REFLECTION_TYPE_DIFFUSE))
    ));
    
    camera = RCO::rc0<Camera>(new Camera());
    camera->setLookat(
        Vec(50.0, 52.0, 220.0),
        Vec(50.0, 52.0 - 0.04, 220.0 - 1.0),
        Vec(0.0, 1.0, 0.0)
    );
    camera->setFocal(aspect, 40.0, 30.0 * aspect);
}

inline bool Scene::intersect_scene(const Ray &ray, Intersection *intersection) {
    intersection->hitpoint_.distance_ = kINF;
    intersection->object_id_ = -1;
    
    // sphere linear search
    for(unsigned int i = 0; i < objects.size(); i++) {
        Hitpoint hitpoint;
        if(objects[i]->geometry->intersect(ray, &hitpoint)) {
            if(hitpoint.distance_ < intersection->hitpoint_.distance_) {
                intersection->hitpoint_ = hitpoint;
                intersection->object_id_ = i;
            }
        }
    }
    
    return (intersection->object_id_ != -1);
}

// from radiance.h
Color Scene::radiance(const Ray &ray, Random *rnd, const int depth) {
    Intersection intersection;
    
    if(!intersect_scene(ray, &intersection)) {
        return BackgroundColor;
    }
    
    //const Object *now_object = objects[intersection.object_id_];
    const Hitpoint &hitpoint = intersection.hitpoint_;
    const Vec orienting_normal = (dot(hitpoint.normal_, ray.dir_) < 0.0)? hitpoint.normal_ : (-1.0 * hitpoint.normal_);
    
    //assert(hitpoint.material!=NULL);
    
    Material *now_mat = hitpoint.material;
    Material::Coord dummyco;
    const Color now_color = now_mat->albedo(dummyco);
    const Color now_emission = now_mat->emission(dummyco);
    const ReflectionType now_reftype = now_mat->reflectionType(dummyco);
    
    //+++++
    return now_emission + now_color;
    //+++++
    
    // max color component for russian roulette.
    double russian_roulette_probability = std::max(now_color.x_, std::max(now_color.y_, now_color.z_));
    // avoid stack overflow
    if(depth > DepthLimit) {
        russian_roulette_probability *= pow(0.5, depth - DepthLimit);
    }
    
    // if depth is larger than minimum depth, do roussian roulette.
    if(depth > Depth) {
        if(rnd->next01() >= russian_roulette_probability) {
            return now_emission;
        }
    } else {
        russian_roulette_probability = 1.0;
    }
    
    // BSDF
    Color incoming_radiance;
    Color weight = 1.0;
    
    switch(now_reftype) {
        case REFLECTION_TYPE_DIFFUSE: {
            Vec w, u, v;
            w = orienting_normal;
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
            
            incoming_radiance = radiance(Ray(hitpoint.position_, dir), rnd, depth + 1);
            weight = now_color / russian_roulette_probability;
        }
            break;
        case REFLECTION_TYPE_SPECULAR: {
            incoming_radiance = radiance(Ray(hitpoint.position_, ray.dir_ - hitpoint.normal_ * 2.0 * dot(hitpoint.normal_, ray.dir_)), rnd, depth + 1);
            weight = now_color / russian_roulette_probability;
        }
            break;
        case REFLECTION_TYPE_REFRACTION:{
            const Ray reflection_ray = Ray(hitpoint.position_, ray.dir_ - hitpoint.normal_ * 2.0 * dot(hitpoint.normal_, ray.dir_));
            const bool into = dot(hitpoint.normal_, orienting_normal) > 0.0;
            // Snell's law
            const double nc = 1.0;
            const double nt = kIor;
            const double nnt = into ? (nc / nt) : (nt / nc);
            const double ddn = dot(ray.dir_, orienting_normal);
            const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);
            
            // full reflection
            if(cos2t < 0.0) {
                incoming_radiance = radiance(reflection_ray, rnd, depth + 1);
                weight = now_color / russian_roulette_probability;
                break;
            }
            
            // refraction
            const Ray refraction_ray = Ray(
                hitpoint.position_,
                normalize(ray.dir_ * nnt - hitpoint.normal_ * (into ? 1.0 : -1.0) * (ddn * nnt + sqrt(cos2t)))
                );
            
            // Schlick' Fresnel aproximation
            const double a = nt - nc, b = nt + nc;
            const double R0 = (a * a) / (b * b);
            
            const double c = 1.0 - (into ? -ddn : dot(refraction_ray.dir_, -1.0 * orienting_normal));
            const double Re = R0 + (1.0 - R0) * pow(c, 5.0);
            const double nnt2 = pow(into ? (nc / nt) : (nt / nc), 2.0);
            const double Tr = (1.0 - Re) * nnt2;
            
            // In deep step case, choose reflect or refract by russian roulette.
            const double probability = 0.25 + 0.5 * Re;
            if(depth > 2) {
                if(rnd->next01() < probability) { // reflection
                    incoming_radiance = radiance(reflection_ray, rnd, depth + 1) * Re;
                    weight = now_color / ((1.0 - probability) * russian_roulette_probability);
                } else { // refraction
                    incoming_radiance = radiance(refraction_ray, rnd, depth + 1) * Tr;
                    weight = now_color / ((1.0 - probability) * russian_roulette_probability);
                }
            } else { // shallow case, calculate both
                incoming_radiance =
                    radiance(reflection_ray, rnd, depth + 1) * Re +
                    radiance(refraction_ray, rnd, depth + 1) * Tr;
                weight = now_color / russian_roulette_probability;
            }
        }
            break;
    }
    
    return now_emission + multiply(weight, incoming_radiance);
    //return Color(rand->next01(), rand->next01(), rand->next01());
    //return Color(1.0, 0.5, 0.0);
}
