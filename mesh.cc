
#include "mesh.h"

using namespace r1h;

void Mesh::calcSmoothNormals() {
    // clear normals
    for(int i = 0; i < normals.size(); i++) {
        normals[i] = Vec(0.0, 0.0, 0.0);
    }
    // calc face normal
    for(int i = 0; i < faces.size(); i++) {
        const Face &f = faces[i];
        const Vec v01 = vertices[f.v1] - vertices[f.v0];
        const Vec v02 = vertices[f.v2] - vertices[f.v0];
        const Vec nv = cross(v01, v02);
        normals[f.n0] = normals[f.n0] + nv;
        normals[f.n1] = normals[f.n1] + nv;
        normals[f.n2] = normals[f.n2] + nv;
    }
    // normalize
    for(int i = 0; i < normals.size(); i++) {
        normals[i] = normalize(normals[i]);
    }
}

bool Mesh::triangleIntersect(const int faceid, const Ray &ray, TriangleHitInfo *hitinfo) const {
    const Face &face = faces[faceid];
    const Vec &v0 = vertices[face.v0];
    const Vec &v1 = vertices[face.v1];
    const Vec &v2 = vertices[face.v2];
    
    const Vec v01 = v1 - v0;
    const Vec v02 = v2 - v0;
    const Vec r = ray.org_ - v0;
    const Vec u = cross(ray.dir_, v02);
    const Vec v = cross(r, v01);
    
    const double div = 1.0 / dot(u, v01);
    const double t = dot(v, v02) * div;
    const double b = dot(u, r) * div;
    const double c = dot(v, ray.dir_) * div;
    
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
        
        return true;
    }
    return false;
}

bool Mesh::intersect(const Ray &ray, Hitpoint *hitpoint) const {
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
    
    const Face &hitface = faces[nearest_info.faceid];
    hitpoint->distance_ = nearest_info.distance;
    hitpoint->position_ = nearest_info.position;
    hitpoint->normal_ =
        normals[hitface.v0] * nearest_info.w0 +
        normals[hitface.v1] * nearest_info.w1 +
        normals[hitface.v2] * nearest_info.w0;
    //hitpoint->material = (hitface.matid < 0)? 0 : materials[hitface.matid];
    hitpoint->materialId = hitface.matid;
    
    return true;
}
