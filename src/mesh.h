/*

メッシュのデータ構造
vertices[] = {Vec, ...}
normals[] = {Vec, ...}
attributes[] = {{Vec, ...}, ...}
faces[] = {Face, ...}

MapCoord { id mapid, co1, co2, co3; }

Face {
    id v1, v2, v3;
    id n1, n2, n3;
    attributes[] = {MapCoord, ...}
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
#include "aabb.h"
#include "bvhnode.h"

namespace r1h {

struct Mesh : public Geometry {
    
    static const int kTypeID;
    
    struct AttrCoord {
        int attrid;         // Geometryの持つattributesのindex
        int co1, co2, co3;
    };
    
    struct Face {
        int v0, v1, v2;
        int n0, n1, n2;
        std::vector<AttrCoord> attrs;
        int matid;
        
        Face() : matid(-1) {}
        Face(const int a, const int b, const int c, const int m) {
            AttrCoord aco = {0, a, b, c};
            v0 = n0 = a;
            v1 = n1 = b;
            v2 = n2 = c;
            attrs.push_back(aco);
            matid = m;
        }
        
        void setV(const int a, const int b, const int c) {
            v0 = a;
            v1 = b;
            v2 = c;
        }
        void setN(const int a, const int b, const int c) {
            n0 = a;
            n1 = b;
            n2 = c;
        }
        void addAttr(const int attrid, const int a, const int b, const int c) {
            AttrCoord aco = {attrid, a, b, c};
            attrs.push_back(aco);
        }
        
    };
    
    std::vector<Vec> vertices;
    std::vector<Vec> normals;
    std::vector<std::vector<Vec> > attributes;
    std::vector<Face> faces;
    
    BVHNode *bvhRoot;
    
    int vertex_reserved;
    int face_reserved;
    
    // empty mesh
    Mesh(const int vreserve=0, const int freserv=0): Geometry(kTypeID), bvhRoot(0) {
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
        if(bvhRoot) {
            delete bvhRoot;
        }
    }
    
    int addVertexWithAttrs(const Vec &p, const Vec &n, const Vec &uv=0, const int uvid=-1) {
        int ret = addVertex(p);
        addNormal(n);
        if(uvid >= 0) {
            addAttribute(uvid, uv);
        }
        return ret;
    }
    
    int addVertex(const Vec &v) {
        vertices.push_back(v);
        return vertices.size() - 1;
    }
    int addNormal(const Vec &v) {
        normals.push_back(v);
        return normals.size() - 1;
    }
    int newAttributeContainer() {
        std::vector<Vec> attrv;
        attrv.reserve(vertex_reserved);
        attributes.push_back(attrv);
        return attributes.size() - 1;
    }
    int addAttribute(const int attrid, const Vec &v) {
        if(attrid < 0 || attrid > (int)attributes.size()) {
            throw "vertex attributes out of range";
        }
        attributes[attrid].push_back(v);
        return attributes[attrid].size() - 1;
    }
    
    int addFace(const Mesh::Face &f) {
        faces.push_back(f);
        return faces.size() - 1;
    }
    int addFace(const int a, const int b, const int c, const int matid=0) {
        faces.push_back(Face(a, b, c, matid));
        return faces.size() - 1;
    }
    
    Vec getVaryingAttr(const int faceid, const int attrid, const Vec weights);
    
    /////
    struct TriangleHitInfo {
        double distance;
        Vec position;
        double w0, w1, w2;
        int faceid;
        TriangleHitInfo(): distance(kINF), faceid(-1) {}
    };
    
    void calcSmoothNormals();
    void buildBVH();
    
    AABB getAABB() const { return bvhRoot->aabb; }
    
    bool triangleIntersect(const int faceid, const Ray &ray, TriangleHitInfo *hitinfo) const;
    bool intersect(const Ray &ray, Hitpoint *hitpoint) const;
    
private:
    //int recurseBuildBVH(BVHNode &node, AABB *aabbPtrs, const int aabbnum, const int depth=0);
    bool intersectBVHNode(const BVHNode &node, const Ray &ray, TriangleHitInfo *hitinfo) const;
};

}
#endif
