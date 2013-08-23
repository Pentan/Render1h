#include <iostream>
#include <cstdio>
#include <map>

#include "scene.h"
#include "sphere.h"
#include "object.h"
#include "mesh.h"
#include "camera.h"
#include "eduptmaterial.h"
#include "wavefrontobj.h"
#include "bsdf.h"
#include "diffusebsdf.h"
#include "aabbgeometry.h"
#include "background.h"
#include "texture.h"

#include "mainscene.h"

/////
namespace r1h {

class MainCubeMaterial : public Material {
public:
    BSDF *bsdf;
    VoronoiTexture *voronoi;
    
    MainCubeMaterial() : bsdf(0) {
        bsdf = new DiffuseBSDF();
        voronoi = new VoronoiTexture(0.8, time(NULL));
        voronoi->setTransform(Mat4::makeScale(10.0, 10.0, 10.0));
    }
    ~MainCubeMaterial() {
        delete bsdf;
        delete voronoi;
    }
    
    Color albedo(const Object &obj, const Hitpoint &hp) const {
        double d = 1.0;
        if(obj.geometry->typeID == Mesh::kTypeID) {
            Mesh *mesh = dynamic_cast<Mesh*>(obj.geometry);
            Vec vuv = mesh->getVaryingAttr(hp.faceId, 0, hp.varyingWeight);
            d = vuv.x_;
        }
        return Color(0.8, 0.6, 0.4) * (d * 0.5 + 0.5);
    }
    Color emission(const Object &obj, const Hitpoint &hp) const {
        
        Color voro = voronoi->sample(hp.position_);
        double vo = voro.x_ * voro.x_;
        return Color(0.0, 0.0, 0.0) * vo;
    }
    void calcNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<TraceInfo> &outvecs) {
        bsdf->getNextRays(ray, hp, depth, rnd, outvecs);
    }
};

class MainPlaneMaterial : public Material {
public:
    BSDF *bsdf;
    VoronoiTexture *voronoi;
    
    MainPlaneMaterial(): bsdf(0) {
        bsdf = new DiffuseBSDF();
        voronoi = new VoronoiTexture(0.9, time(NULL));
        voronoi->setTransform(Mat4::makeScale(4.0, 4.0, 4.0));
    }
    ~MainPlaneMaterial() {
        delete bsdf;
        delete voronoi;
    }
    
    Color albedo(const Object &obj, const Hitpoint &hp) const {
        return Color(0.1, 0.3, 0.0);
    }
    Color emission(const Object &obj, const Hitpoint &hp) const {
        Color voro = voronoi->sample(hp.position_);
        return (voro.x_ > 0.7)? Color(2.0, 1.0, 0.5) : Color(0.0, 0.0, 0.0);
    }
    void calcNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<TraceInfo> &outvecs) {
        bsdf->getNextRays(ray, hp, depth, rnd, outvecs);
    }
};

class MainModelLoader : public WavefrontObj {
    
public:
    Mesh *mesh;
    
    MainModelLoader(const char *filepath) : WavefrontObj(filepath), mesh(0) {
        mesh = new Mesh(1000, 1000);
        mesh->newAttributeContainer();
        mesh->addVertexWithAttrs(Vec(0.0), Vec(0.0), Vec(0.0), 0);
    }
    ~MainModelLoader() {
        mesh->release();
    }
    
    // v, vn, vt, Ns, Ka, Kd, Ks, Ni, d
    void foundVector(const ParameterType pt, const double x, const double y, const double z) {
        switch(pt) {
            case OBJ_v:
                //mesh->addVertex(Vec(x, y, z));
                mesh->addVertexWithAttrs(Vec(x, y, z), Vec(0.0), Vec(x, y, z), 0);
                //std::cout << "v " << x << "," << y << "," << z << std::endl;
                break;
            case OBJ_vn:
                //mesh->addNormal(Vec(x, y, z));
                //std::cout << "n " << x << "," << y << "," << z << std::endl;
                break;
            case OBJ_vt:
                //mesh->addVertexWithAttrs(Vec(x, y, z), 0);
                //std::cout << "t " << x << "," << y << "," << z << std::endl;
                break;
            case MTL_Ns:
            case MTL_Ka:
            case MTL_Kd:
            case MTL_Ks:
            case MTL_Ni:
            case MTL_d:
            default:
                break;
        }
    }
    
    // mtllib, o, s, usemtl, newmtl, MTL_map_Kd
    void foundString(const ParameterType pt, const std::string &str) {
        switch(pt) {
            case OBJ_o:
            case OBJ_g:
            case OBJ_s:
            case OBJ_usemtl:
            case OBJ_mtllib:
            case MTL_newmtl:
            case MTL_map_Kd:
            default:
                break;
        }
    }
    
    // illum
    void foundInteger(const ParameterType pt, const int i) {
        //case MTL_illum:
        // noop
    }
    // f
    void foundFace(const ParameterType pt, const std::vector<FaceInfo> &fids) {
        
        // first triangle
        mesh->addFace(fids[0].v, fids[1].v, fids[2].v, 0);
        //std::cout << "f " << fids[0].v << "," << fids[1].v << "," << fids[2].v << std::endl;
        
        if(fids.size() > 3) {
            // quad
            mesh->addFace(fids[2].v, fids[3].v, fids[0].v, 0);
            //std::cout << "f " << fids[2].v << "," << fids[3].v << "," << fids[0].v << std::endl;
        }
    }
    
    // mtl
    void importMTL(const char *filename) {
        WavefrontObj::importMTL(filename);
    }
    
    // endof .mtl or .obj
    virtual void endFile() {
        //std::cout << "obj end" << std::endl;
    }
};
}
/////

using namespace r1h;

// bg
struct GradientBg : public BackgroundMaterial {
    Color backgroundColor(const Ray &ray) {
        Vec upv = Vec::normalized(Vec(1.0, 1.0, 0.0));
        Color sun = Color(10.0) * pow(fabs(Vec::dot(ray.dir_, upv)), 8.0);
        Color grad = Color::lerp(Color(0.2, 0.5, 0.8), Color(0.4, 0.2, 0.1), fabs(ray.dir_.y));
        return (sun + grad * 0.5);
    }
};

//
static void setupCubes(Scene &scene) {
    // objects
    const int objNum = 8;
    const int noiseseed = time(NULL);
    
    Random rnd(time(NULL));
    
    for(int iobj = 0; iobj < objNum; iobj++) {
        Object *obj;
        
        //----- cube
        MainModelLoader cubeloader("divcube.obj");
        cubeloader.setBasePath("../bin/model");
        cubeloader.load();
        
        // some mesh operation?
        Mesh *mesh = cubeloader.mesh;
        
        mesh->calcSmoothNormals();
        FractalNoiseTexture *displace = new FractalNoiseTexture(FractalNoiseTexture::SmoothNoise, 6, noiseseed + iobj);
        displace->setTransform(Mat4::makeScale(2.0, 2.0, 2.0));
        
        Vec rotaxis = Vec::normalized(Vec(1.0));
        //Mat4 trns = Mat4::makeRotation(1.0, rotaxis.x_, rotaxis.y_, rotaxis.z_);
        Mat4 trns = Mat4::makeRotation(((iobj + 0.5) / objNum) * -3.14159, 0.0, 1.0, 0.0);
        trns.translate(4.0 + rnd.next01() * 4.0, 0.0, 0.0);
        trns.rotate(rnd.next01(), rotaxis);
        
        for(int i = 0; i < (int)mesh->vertices.size(); i++) {
            Vec &v  = mesh->vertices[i];
            Vec &vn = mesh->normals[i];
            Vec &vt = mesh->attributes[0][i];
            Color noise = displace->sample(v);
            double d = noise.r;
            
            v += vn * (d * 0.75);
            v = Mat4::transformV3(trns, v);
            
            vt.x_ = d;
        }
        delete displace;
    
        
        // finish
        cubeloader.mesh->calcSmoothNormals();
        cubeloader.mesh->buildBVH();
        
        MainCubeMaterial *cubemat = new MainCubeMaterial();
        cubeloader.mesh->addMaterial(cubemat);
        cubemat->release();
        
        // create object
        obj = new Object(cubeloader.mesh);
        scene.addObject(obj);
    }
}

static void setupPlanes(Scene &scene) {
    Object *obj;
    
    //----- plane
    MainModelLoader planeloader("divplane.obj");
    planeloader.setBasePath("../bin/model");
    planeloader.load();
    
    MainPlaneMaterial *planemat = new MainPlaneMaterial();
    planeloader.mesh->addMaterial(planemat);
    planemat->release();
    
    // some mesh operation?
    planeloader.mesh->calcSmoothNormals();
    
    Mesh *mesh = planeloader.mesh;
    FractalNoiseTexture *displace = new FractalNoiseTexture(FractalNoiseTexture::SmoothNoise, 6);
    
    for(int i = 0; i < (int)mesh->vertices.size(); i++) {
        Vec &v  = mesh->vertices[i];
        Vec &vn = mesh->normals[i];
        Vec &vt = mesh->attributes[0][i];
        Color noise = displace->sample(v);
        double d = fabs(noise.r);
        double rr = v.x_ * v.x_ + v.z_ * v.z_;
        
        v += vn * (d * 10.0 * (1.0 - exp(-rr * 0.03)));
        vt.x_ = d;
    }
    delete displace;
    
    
    // finish
    planeloader.mesh->calcSmoothNormals();
    planeloader.mesh->buildBVH();
    
    // create object
    obj = new Object(planeloader.mesh);
    scene.addObject(obj);
}

void r1h::setupMainScene(Scene &scene) {
    
    setupCubes(scene);
    setupPlanes(scene);
    
    //----- background
    GradientBg *bgmat = new GradientBg();
    scene.setBackgroundMaterial(bgmat);
    bgmat->release();
    
    //----- camera
    scene.setCamera(RCO::rc0<Camera>(new Camera()));
    scene.camera->setLookat(
        Vec(0.0, 0.0, -8.0),
        Vec(0.0, 0.0, 0.0),
        Vec(0.0, 1.0, 0.0)
    );
    scene.camera->setFocal(35.0, 32.0);
}
