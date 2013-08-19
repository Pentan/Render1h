/*

メッシュのデータ構造
vertices[] = {Vec, ...}
normals[] = {Vec, ...}
mapcoords[] = {{Vec, ...}, ...}
faces[] = {Face, ...}

MapCoord { id mapid, co1, co2, co3; }

Face {
    id v1, v2, v3;
    id n1, n2, n3;
    coords[] = {MapCoord, ...}
    Material *m
}

*/

#ifndef R1H_MESH_H
#define R1H_MESH_H

#include "constants.h"
#include "vec.h"
#include "ray.h"
#include "material.h"
#include "intersection.h"
#include "geometry.h"

namespace r1h {

struct Mesh : public Geometry {
    struct MapCoord {
        int mapid;
        int co1, co2, co3;
    };
    
    struct Face {
        int v0, v1, v2;
        int n0, n1, n2;
        std::vector<MapCoord> coords;
        int matid;
        
        Face(const int a, const int b, const int c, const int m) {
            MapCoord mco = {0, a, b, c};
            v0 = n0 = a;
            v1 = n1 = b;
            v2 = n2 = c;
            coords.push_back(mco);
            matid = m;
        }
    };
    
    std::vector<Vec> vertices;
    std::vector<Vec> normals;
    std::vector<std::vector<Vec> > coords;
    std::vector<Face> faces;
    
    int vertex_reserved;
    int face_reserved;
    
    // empty mesh
    Mesh(const int vreserve=0, const int freserv=0): Geometry() {
        if(vreserve > 0) {
            vertices.reserve(vreserve);
            normals.reserve(vreserve);
        }
        if(freserv > 0) {
            faces.reserve(freserv);
        }
        vertex_reserved = vreserve;
        face_reserved = freserv;
    }
    
    ~Mesh() {
        //printf("Sphere %p destructed\n", this);
    }
    
    int addVertexWithAttrs(const Vec &p, const Vec &n, const Vec &uv=0, const int uvid=-1) {
        vertices.push_back(p);
        normals.push_back(n);
        if(uvid > 0) {
            if(coords.size() <= uvid) {
                std::vector<Vec> cov;
                cov.reserve(vertex_reserved);
                coords.push_back(cov);
            }
            coords[uvid].push_back(uv);
        }
        return vertices.size() - 1;
    }
    
    int addFace(const Mesh::Face &f) {
        faces.push_back(f);
        return faces.size() - 1;
    }
    int addFace(const int a, const int b, const int c, const int matid=0) {
        faces.push_back(Face(a, b, c, matid));
        return faces.size() - 1;
    }
    
    struct TriangleHitInfo {
        double distance;
        Vec position;
        double w0, w1, w2;
        int faceid;
        TriangleHitInfo(): distance(kINF), faceid(-1) {}
    };
    
    void calcSmoothNormals();
    
    bool triangleIntersect(const int faceid, const Ray &ray, TriangleHitInfo *hitinfo) const;
    bool intersect(const Ray &ray, Hitpoint *hitpoint) const;
    
    /*
    bool triangleIntersect(const int faceid, const Ray &ray, TriangleHitInfo *hitinfo) {
        // TODO
    }
    
    bool intersect(const Ray &ray, Hitpoint *hitpoint) const {
        // brute force now!
        TriangleHitInfo nearest_info, tmp_info;
        for(int iface = 0; iface < faces.size(); iface++) {
            if(triangleIntersect(iface, ray, &tmp_info)) {
                if(tmp_info->distance < nearest_info->distance) {
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
        hitpoint->material = hitface.mat;
        
        return true;
    }
    */
};

}
#endif
