
#include <algorithm>
#include "mesh.h"

using namespace r1h;

const int Mesh::kTypeID = 'MESH';

Vec Mesh::getVaryingAttr(const int faceid, const int attrid, const Vec weights) {
    const Face &face = faces[faceid];
    const AttrCoord &attrco = face.attrs[attrid];
    const std::vector<Vec> &attrvec = attributes[attrco.attrid];
    const Vec a1 = attrvec[attrco.co1];
    const Vec a2 = attrvec[attrco.co2];
    const Vec a3 = attrvec[attrco.co3];
    
    return a1 * weights.x_ + a2 * weights.y_ + a3 * weights.z_;
}

void Mesh::calcSmoothNormals() {
    // clear normals
    for(int i = 0; i < (int)normals.size(); i++) {
        normals[i] = Vec(0.0, 0.0, 0.0);
    }
    // calc face normal
    for(int i = 0; i < (int)faces.size(); i++) {
        const Face &f = faces[i];
        const Vec v01 = vertices[f.v1] - vertices[f.v0];
        const Vec v02 = vertices[f.v2] - vertices[f.v0];
        const Vec nv = Vec::cross(v01, v02);
        normals[f.n0] = normals[f.n0] + nv;
        normals[f.n1] = normals[f.n1] + nv;
        normals[f.n2] = normals[f.n2] + nv;
    }
    // normalize
    for(int i = 0; i < (int)normals.size(); i++) {
        normals[i] = Vec::normalized(normals[i]);
    }
}

void Mesh::buildBVH() {
    if(bvhRoot) {
        // rebuild?
        //std::cout << "rebuild BVH ?" << std::endl;
        delete bvhRoot;
        bvhRoot = 0;
    }
    
    int facenum = faces.size();
    std::cout  << "faces:" << facenum << std::endl;
    
    AABB *faceAABBs = new AABB[facenum];
    for(int i = 0; i < facenum; i++) {
        Face &fc = faces[i];
        AABB &ab = faceAABBs[i];
        ab.expand(vertices[fc.v0]);
        ab.expand(vertices[fc.v1]);
        ab.expand(vertices[fc.v2]);
        ab.dataId = i;
    }
    bvhRoot = new BVHNode();
    //int maxdepth = recurseBuildBVH(*bvhRoot, faceAABBs, facenum);
    int maxdepth = bvhRoot->buildAABBTree(faceAABBs, facenum);
    
    std::cout << "max BVH depth:" << maxdepth << std::endl;
    
    delete [] faceAABBs;
}

bool Mesh::triangleIntersect(const int faceid, const Ray &ray, TriangleHitInfo *hitinfo) const {
    const Face &face = faces[faceid];
    const Vec &v0 = vertices[face.v0];
    const Vec &v1 = vertices[face.v1];
    const Vec &v2 = vertices[face.v2];
    
    const Vec v01 = v1 - v0;
    const Vec v02 = v2 - v0;
    const Vec r = ray.org_ - v0;
    const Vec u = Vec::cross(ray.dir_, v02);
    const Vec v = Vec::cross(r, v01);
    
    const double div = 1.0 / Vec::dot(u, v01);
    const double t = Vec::dot(v, v02) * div;
    const double b = Vec::dot(u, r) * div;
    const double c = Vec::dot(v, ray.dir_) * div;
    
    /*
    printf("face(%d,%d,%d)\n", face.v0, face.v1, face.v2);
    v0.print("v0");
    v1.print("v1");
    v2.print("v2");
    v01.print("v01");
    v02.print("v02");
    ray.dir_.print("ray.dir_");
    ray.org_.print("ray.org_");
    r.print("r");
    u.print("u");
    v.print("v");
    printf("(t,b,c)=(%lf,%lf,%lf)\n", t, b, c);//+++++
    */
    if((b + c < 1.0 && b > 0.0 && c > 0.0) && t > 0.0) {
        // hit
        const double a = 1.0 - b - c;
        hitinfo->distance = t;
        hitinfo->position = v0 * a + v1 * b + v2 * c;
        hitinfo->w0 = a;
        hitinfo->w1 = b;
        hitinfo->w2 = c;
        hitinfo->faceid = faceid;
        
        return true;
    }
    return false;
}

bool Mesh::intersectBVHNode(const BVHNode &node, const Ray &ray, TriangleHitInfo *hitinfo) const {
    if(node.isLeaf()) {
        // 葉ノード
        TriangleHitInfo tmp_info;
        if(triangleIntersect(node.dataId, ray, &tmp_info)) {
            if(tmp_info.distance < hitinfo->distance) {
                *hitinfo = tmp_info;
                return true;
            }
        }
        //return false; // at last
    } else {
        double d;
        if(node.aabb.isIntersect(ray, &d)) { // レイがAABBにヒットしている
            if(d < hitinfo->distance) { // より近くで
                TriangleHitInfo nearest_info, tmp_info;
                for(int i = 0; i < node.childNum; i++) {
                    // 子ノードをチェック
                    if(intersectBVHNode(node.children[i], ray, &tmp_info)) {
                        if(tmp_info.distance < nearest_info.distance) {
                            nearest_info = tmp_info;
                        }
                    }
                }
                // 今のヒット位置よりも近かったら採用
                if(nearest_info.distance < hitinfo->distance) {
                    *hitinfo = nearest_info;
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool Mesh::intersect(const Ray &ray, Hitpoint *hitpoint) const {
    
#if 0
    /////+++++
    // brute force now!
    TriangleHitInfo nearest_info, tmp_info;
    for(int iface = 0; iface < faces.size(); iface++) {
        //printf("face[%d]\n", iface);//+++++
        if(triangleIntersect(iface, ray, &tmp_info)) {
            if(tmp_info.distance < nearest_info.distance) {
                nearest_info = tmp_info;
                nearest_info.faceid = iface;
            }
        }
    }
    if(nearest_info.faceid < 0) {
        return false;
    }
    /////+++++
#else
    /////+++++
    // traverse BVH
    TriangleHitInfo nearest_info;
    if(!intersectBVHNode(*bvhRoot, ray, &nearest_info)) {
        return false;
    }
    /////+++++
#endif
    
    // calc face infomation
    const Face &hitface = faces[nearest_info.faceid];
    hitpoint->distance_ = nearest_info.distance;
    hitpoint->position_ = nearest_info.position;
    hitpoint->normal_ =
        normals[hitface.v0] * nearest_info.w0 +
        normals[hitface.v1] * nearest_info.w1 +
        normals[hitface.v2] * nearest_info.w2;
    // TODO: attributes
    //hitpoint->material = (hitface.matid < 0)? 0 : materials[hitface.matid];
    hitpoint->materialId = hitface.matid;
    hitpoint->varyingWeight = Vec(nearest_info.w0, nearest_info.w1, nearest_info.w2);
    
    return true;
}
