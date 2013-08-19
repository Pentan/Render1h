
#include <cassert>
#include "scene.h"

#include "mesh.h"
#include "eduptmaterial.h"

using namespace r1h;

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

#if 0
// original recursive routine
// from radiance.h
Color Scene::radiance(const Ray &ray, Random *rnd, const int depth) {
    Intersection intersection;
    
    if(!intersect_scene(ray, &intersection)) {
        return BackgroundColor;
    }
    
    const Object *now_object = objects[intersection.object_id_];
    const Hitpoint &hitpoint = intersection.hitpoint_;
    const Vec orienting_normal = (dot(hitpoint.normal_, ray.dir_) < 0.0)? hitpoint.normal_ : (-1.0 * hitpoint.normal_);
    
    //assert(hitpoint.material!=NULL);
    
    Material *now_mat = now_object->getMaterialForId(hitpoint.materialId);
    assert(now_mat);
    
    const Color now_color = now_mat->albedo(hitpoint);
    const Color now_emission = now_mat->emission(hitpoint);
    const ReflectionType now_reftype = now_mat->reflectionType(hitpoint);
    
    //+++++
    //return now_emission + now_color;
    //return hitpoint.normal_ * 0.5 + Vec(0.5, 0.5, 0.5);
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

//+++++
#else
// my loop routine
// from radiance.h
Color Scene::radiance(const Ray &ray, Random *rnd, const int firstdepth) {
    // いろいろ用意
    std::vector<TraceInfo> rays1, rays2, tmprays;
    rays1.reserve(128);
    rays2.reserve(128);
    tmprays.reserve(4);
    std::vector<TraceInfo> *currays, *nextrays, *tmpraysptr;
    currays = &rays1;
    nextrays = &rays2;
    
    // ラディアンスを初期化
    Color radiance_color = Color(0.0);
    // 最初のレイ
    currays->push_back(TraceInfo(ray, Color(1.0)));
    
    // 追跡すべきレイがある限り
    int depth = firstdepth;
    while(currays->size() > 0) {
        // 次のレイのコンテナを初期化
        nextrays->clear();
        // 追跡
        for(int i = 0; i < currays->size(); i++) {
            const TraceInfo &tracer = currays->at(i);
            
            Intersection intersection;
            
            // 交差しなければ背景色
            if(!intersect_scene(tracer.ray, &intersection)) {
                radiance_color += multiply(BackgroundColor, tracer.weight);
                continue;
            }
            
            // 当たったオブジェクト
            const Object *now_object = objects[intersection.object_id_];
            const Hitpoint &hitpoint = intersection.hitpoint_;
            
            // 当たった先の表面情報から
            Material *now_mat = now_object->getMaterialForId(hitpoint.materialId);
            assert(now_mat);
            const Color now_color = now_mat->albedo(hitpoint);
            const Color now_emission = now_mat->emission(hitpoint);
            
            // 放射に重みをかけて足す
            radiance_color += multiply(now_emission, tracer.weight);
            
            // ロシアンルーレット
            double russian_roulette_probability = std::max(now_color.x_, std::max(now_color.y_, now_color.z_));
            // avoid stack overflow
            if(depth > DepthLimit) {
                russian_roulette_probability *= pow(0.5, depth - DepthLimit);
            }
            // if depth is larger than minimum depth, do roussian roulette.
            if(depth > Depth) {
                if(rnd->next01() >= russian_roulette_probability) {
                    continue;
                }
            } else {
                russian_roulette_probability = 1.0;
            }
            
            // 次のレイのための重みを計算
            const Color nextweight = now_color / russian_roulette_probability;
            
            // 次に追跡すべきレイを取得
            tmprays.clear();
            now_mat->calcNextRays(tracer.ray, hitpoint, depth, rnd, tmprays);
            // 現在の重みを考慮しつつ次のリストへ追加
            for(int j = 0; j < tmprays.size(); j++) {
                TraceInfo jnext = tmprays[j];
                jnext.weight = multiply(jnext.weight, multiply(tracer.weight, nextweight));
                nextrays->push_back(jnext);
            }
        }
        // コンテナを入れ替えて次へ
        tmpraysptr = currays;
        currays = nextrays;
        nextrays = tmpraysptr;
        ++depth;
    }
    
    return radiance_color;
}
#endif
//+++++


