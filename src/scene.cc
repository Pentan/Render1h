
#include <cassert>
#include "scene.h"

#include "mesh.h"
#include "eduptmaterial.h"

using namespace r1h;

void Scene::prepareRender() {
    if(objectsBVH) {
        delete objectsBVH;
    }
    
    int objnum = objects.size();
    AABB *aabbArray = new AABB[objnum];
    for(int i = 0; i < objnum; i++) {
        aabbArray[i] = objects[i]->geometry->getAABB();
        aabbArray[i].dataId = i;
    }
    objectsBVH = new BVHNode();
    int maxdepth = objectsBVH->buildAABBTree(aabbArray, objnum);
    delete [] aabbArray;
    //std::cout << "objects:" << objnum << ", BVH depth:" << maxdepth << std::endl;
}

bool Scene::intersect_scene(const Ray &ray, Intersection *intersection) {
    intersection->hitpoint_.distance_ = kINF;
    intersection->object_id_ = -1;
    
#if 1
    // �Ƃ肠�����I�u�W�F�N�g��������
    for(unsigned int i = 0; i < objects.size(); i++) {
        Hitpoint hitpoint;
        if(objects[i]->geometry->intersect(ray, &hitpoint)) {
            if(hitpoint.distance_ < intersection->hitpoint_.distance_) {
                intersection->hitpoint_ = hitpoint;
                intersection->object_id_ = i;
            }
        }
    }
#else
    // BVH�o�[�W����(TODO
    intersectBVHNode(*objectsBVH, ray, intersection);
#endif
    
    return (intersection->object_id_ != -1);
}

bool Scene::intersectBVHNode(const BVHNode &node, const Ray &ray, Intersection *intersection) {
    if(node.isLeaf()) {
        //�t
        Hitpoint hitpoint;
        if(objects[node.dataId]->geometry->intersect(ray, &hitpoint)) {
            if(hitpoint.distance_ < intersection->hitpoint_.distance_) {
                intersection->hitpoint_ = hitpoint;
                intersection->object_id_ = node.dataId;
            }
        }
    } else {
        double d;
        if(node.aabb.isIntersect(ray, &d)) { // ���C��������AABB�Ƀq�b�g���Ă���
            if(d < intersection->hitpoint_.distance_) { // ���߂���
                Intersection nearestisect = *intersection;
                Intersection tmpisect = *intersection;
                for(int i = 0; i < node.childNum; i++) {
                    // �q�m�[�h���`�F�b�N
                    if(intersectBVHNode(node.children[i], ray, &tmpisect)) {
                        if(tmpisect.hitpoint_.distance_ < nearestisect.hitpoint_.distance_) {
                            nearestisect = tmpisect;
                        }
                    }
                }
                // ���̃q�b�g�ʒu�����߂�������̗p
                if(nearestisect.hitpoint_.distance_ < intersection->hitpoint_.distance_) {
                    *intersection = nearestisect;
                    return true;
                }
            }
        }
    }
    return false;
}

Color Scene::radiance(const Ray &ray, RenderContext *rndrcntx, const int firstdepth) {
    // ���낢��p��
    /*
    std::vector<TraceInfo> rays1, rays2, rays3;
    rays1.reserve(128);
    rays2.reserve(128);
    rays3.reserve(4);
    */
    
    std::vector<TraceInfo> *currays, *nextrays, *tmprays, *swaptmpptr;
    
    Random *rnd = &rndrcntx->random;
    //std::vector<TraceInfo> *currays, *nextrays, *tmprays, *swaptmpptr;
    currays = &rndrcntx->traceVec1;
    nextrays = &rndrcntx->traceVec2;
    tmprays = &rndrcntx->workVec;
    
    currays->clear();
    nextrays->clear();
    tmprays->clear();
    
    // ���f�B�A���X��������
    Color radiance_color = Color(0.0);
    // �ŏ��̃��C
    currays->push_back(TraceInfo(ray, Color(1.0)));
    
    // �ǐՂ��ׂ����C���������
    int depth = firstdepth;
    while(currays->size() > 0) {
        // ���̃��C�̃R���e�i��������
        nextrays->clear();
        // �ǐ�
        int curmax = currays->size();
        for(int i = 0; i < curmax; i++) {
            const TraceInfo &tracer = currays->at(i);
            
            Intersection intersection;
            
            // �������Ȃ���Δw�i�F
            if(!intersect_scene(tracer.ray, &intersection)) {
                Color bgcol = (background)? background->backgroundColor(tracer.ray) : Color(0.0);
                radiance_color += Color::mul(bgcol, tracer.weight);
                continue;
            }
            
            // ���������I�u�W�F�N�g
            const Object *now_object = objects[intersection.object_id_];
            const Hitpoint &hitpoint = intersection.hitpoint_;
            
            // ����������̕\�ʏ�񂩂�
            Material *now_mat = now_object->getMaterialForId(hitpoint.materialId);
            assert(now_mat);
            const Color now_color = now_mat->albedo(*now_object, hitpoint);
            const Color now_emission = now_mat->emission(*now_object, hitpoint);
            
            // ���˂ɏd�݂������đ���
            radiance_color += Color::mul(now_emission, tracer.weight);
            
            //+++++ normal color
            //radiance_color += hitpoint.normal_ * 0.5 + Vec(0.5);
            //continue;
            //+++++
            
            // ���V�A�����[���b�g
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
            
            // ���̃��C�̂��߂̏d�݂��v�Z
            const Color nextweight = now_color / russian_roulette_probability;
            
            // ���ɒǐՂ��ׂ����C���擾
            tmprays->clear();
            now_mat->calcNextRays(tracer.ray, hitpoint, depth, rnd, *tmprays);
            // ���݂̏d�݂��l�������̃��X�g�֒ǉ�
            for(int j = 0; j < (int)tmprays->size(); j++) {
                TraceInfo &jnext = tmprays->at(j);
                jnext.ray.smallOffset(hitpoint.normal_);
                jnext.weight = Color::mul(jnext.weight, Color::mul(tracer.weight, nextweight));
                nextrays->push_back(jnext);
            }
        }
        // �R���e�i�����ւ��Ď���
        swaptmpptr = currays;
        currays = nextrays;
        nextrays = swaptmpptr;
        ++depth;
    }
    
    return radiance_color;
}


