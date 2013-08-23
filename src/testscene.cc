
#include <iostream>
#include <cstdio>
#include <map>

#include "testscene.h"
#include "sphere.h"
#include "object.h"
#include "mesh.h"
#include "camera.h"
#include "eduptmaterial.h"
#include "wavefrontobj.h"
#include "bsdf.h"
#include "diffusebsdf.h"
#include "aabbgeometry.h"

namespace r1h {

class TestObjMaterial : public Material {
public:
    Color diffuse;
    Color ambient;
    BSDF *bsdf;
    
    TestObjMaterial() : diffuse(0.8, 0.8, 0.8), ambient(0.001, 0.001, 0.001) {
        bsdf = new DiffuseBSDF();
    }
    ~TestObjMaterial() {
        delete bsdf;
    }
    
    Color albedo(const Object &obj, const Hitpoint &hp) const {
        return diffuse;
    }
    Color emission(const Object &obj, const Hitpoint &hp) const {
        return ambient;
    }
    void calcNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<TraceInfo> &outvecs) {
        bsdf->getNextRays(ray, hp, depth, rnd, outvecs);
    }
};

class TestObjImport : public WavefrontObj {
    Scene *scene;
    
    // context
    std::vector<Vec> vBuf;
    std::vector<Vec> vnBuf;
    std::vector<Vec> vtBuf;
    int insideMTL;
    
    // for obj
    Object *curObject;
    Mesh *curMesh;
    bool isSmooth;
    int uvattrId;
    int curMatId;
    int objVnCount;
    // for mtl
    TestObjMaterial *curMat;
    // common
    std::map<std::string, TestObjMaterial*> nameMatMap;
    
    void finishCurrentObject() {
        if(curObject) {
            //printf("finish object:%p\n", curObject);
            //std::cout << "finish object. smooth:" << isSmooth << std::endl;
            
            // 既に前に宣言された法線を使っている場合はおかしくなるが気にしないことにする
            //if(objVnCount <= 0 || isSmooth) {
            if(objVnCount <= 0) {
                //std::cout << "calc normal" << std::endl;
                curMesh->calcSmoothNormals();
            }
            
            //std::cout << "buildBVH" << std::endl;
            curMesh->buildBVH();
            
            curObject = 0;
            curMesh = 0;
            isSmooth = false;
            uvattrId = 0;
        }
    }
    
public:
    TestObjImport(Scene *sc, const char *filepath) : WavefrontObj(filepath), scene(sc), curObject(0), curMesh(0), isSmooth(true), uvattrId(0) {
        vBuf.reserve(1024);
        vnBuf.reserve(1024);
        vtBuf.reserve(1024);
        vBuf.push_back(Vec());
        vnBuf.push_back(Vec());
        vtBuf.push_back(Vec());
        insideMTL = 0;
    }
    ~TestObjImport() {
        std::map<std::string, TestObjMaterial*>::iterator ite = nameMatMap.begin();
        while(ite != nameMatMap.end()) {
            TestObjMaterial *matptr = (*ite).second;
            matptr->release();
            ++ite;
        }
    }
    
    // v, vn, vt, Ns, Ka, Kd, Ks, Ni, d
    void foundVector(const ParameterType pt, const double x, const double y, const double z) {
        switch(pt) {
            case OBJ_v:
                vBuf.push_back(Vec(x, y, z));
                //std::cout << "v " << x << "," << y << "," << z << std::endl;
                break;
            case OBJ_vn:
                vnBuf.push_back(Vec(x, y, z));
                ++objVnCount;
                //std::cout << "n " << x << "," << y << "," << z << std::endl;
                break;
            case OBJ_vt:
                vtBuf.push_back(Vec(x, y, z));
                //std::cout << "t " << x << "," << y << "," << z << std::endl;
                break;
            case MTL_Ns:
                break;
            case MTL_Ka:
                curMat->ambient = Color(x, y, z);
                //curMat->ambient = Vec(x, y, z) + Vec(0.01); // 光源が無いので
                break;
            case MTL_Kd:
                curMat->diffuse = Color(x, y, z);
                break;
            case MTL_Ks:
                break;
            case MTL_Ni:
                break;
            case MTL_d:
                break;
            default:
                break;
        }
    }
    
    // mtllib, o, s, usemtl, newmtl, MTL_map_Kd
    void foundString(const ParameterType pt, const std::string &str) {
        switch(pt) {
            case OBJ_o:
            case OBJ_g:
                // finish existing
                finishCurrentObject();
                //
                //std::cout << "start object:" << str << std::endl;
                // new object
                curMesh = new Mesh(128, 64);
                curObject = new Object(curMesh);
                curMesh->release();
                // このオブジェクト中に追加されたvnの数
                objVnCount = 0;
                // uv用のattr
                uvattrId = curMesh->newAttributeContainer();
                // とりあえずデフォルトマテリアル
                curMesh->addMaterial(RCO::rc0(new TestObjMaterial()));
                // シーンに追加
                scene->addObject(curObject);
                break;
            case OBJ_s:
                // smooth shading
                isSmooth = (str == "1");
                break;
            case OBJ_usemtl: {
                // マテリアル使用
                TestObjMaterial *tmpmat = nameMatMap[str];
                if(tmpmat) {
                    curMatId = curMesh->addMaterial(tmpmat);
                } else {
                    curMatId = 0;
                }
                
                } break;
            case OBJ_mtllib:
                // mtl
                importMTL(str.c_str());
                break;
            case MTL_newmtl:
                // 新マテリアル
                curMat = new TestObjMaterial();
                nameMatMap[str] = curMat;
                break;
            case MTL_map_Kd:
                break;
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
        int vid[4];
        for(int i = 0; i < 3; i++) {
            vid[i] = curMesh->addVertex(vBuf[fids[i].v]);
            curMesh->addNormal(vnBuf[fids[i].vn]);
            curMesh->addAttribute(uvattrId, vtBuf[fids[i].vt]);
        }
        curMesh->addFace(vid[0], vid[1], vid[2], curMatId);
        
        if(fids.size() > 3) {
            // 三角化.縮退してしまう場合があるがとりあえず気にしない.
            // それと四角以上のときも気にしない
            vid[3] = curMesh->addVertex(vBuf[fids[3].v]);
            curMesh->addNormal(vnBuf[fids[3].vn]);
            curMesh->addAttribute(uvattrId, vtBuf[fids[3].vt]);
            curMesh->addFace(vid[2], vid[3], vid[0], curMatId);
        }
    }
    
    // mtl
    void importMTL(const char *filename) {
        ++insideMTL;
        WavefrontObj::importMTL(filename);
    }
    // endof .mtl or .obj
    virtual void endFile() {
        if(insideMTL > 0) { // end of mtl
            //std::cout << "mtl end" << std::endl;
            //+++++
            std::map<std::string, TestObjMaterial*>::iterator ite = nameMatMap.begin();
            while(ite != nameMatMap.end()) {
                TestObjMaterial *matptr = (*ite).second;
                std::cout << (*ite).first << ":d(";
                std::cout << matptr->diffuse.r << ","<< matptr->diffuse.g << ","<< matptr->diffuse.b << "),a(";
                std::cout << matptr->ambient.r << ","<< matptr->ambient.g << ","<< matptr->ambient.b << ")";
                std::cout << std::endl;
                ++ite;
            }
            //+++++
            curMat = 0;
            --insideMTL;
        } else { // end of obj
            std::cout << "obj end" << std::endl;
            finishCurrentObject();
        }
    }
};
}

using namespace r1h;

void r1h::setupCornelBoxObjScene(Scene &scene, const double aspect) {
    TestObjImport importer(&scene, "cornelbox.obj");
    importer.setBasePath("../bin/model");
    importer.load();
    
    // camera for cornel box
    Camera *camera = new Camera();
    camera->setLookat(
        Vec(50.0, 52.0, 220.0),
        Vec(50.0, 52.0 - 0.04, 220.0 - 1.0),
        Vec(0.0, 1.0, 0.0)
    );
    camera->setFocal(40.0, 30.0 * aspect);
    camera->setAspectRatio(aspect);
    scene.setCamera(camera);
    camera->release();
}

void r1h::setupTestObjScene(Scene &scene, const double aspect) {
    //+++++
    // test obj
    //TestObjImport importer(&scene, "testobj.obj");
    TestObjImport importer(&scene, "testobj2.obj");
    //TestObjImport importer(&scene, "monky3.obj");
    importer.setBasePath("../bin/model");
    importer.load();
    
    scene.BackgroundColor = Color(1.0 ,1.0, 1.0);
    
    scene.setCamera(RCO::rc0<Camera>(new Camera()));
    scene.camera->setLookat(
        Vec(0.0, 0.0, 6.0),
        Vec(0.0, 0.0, 0.0),
        Vec(0.0, 1.0, 0.0)
    );
    scene.camera->setFocal(35.0, 32.0);
    scene.camera->setAspectRatio(aspect);
    //+++++
    /*
    std::cout << "objects:" << scene.objects.size() << std::endl;
    for(int i = 0; i < scene.objects.size(); i++) {
        Object *obj = scene.objects[i];
        Mesh *me = dynamic_cast<Mesh*>(obj->geometry);
        std::cout << "object[" << i << "]" << std::endl;
        std::cout << " vertices:" << me->vertices.size() << std::endl;
        for(int j = 0; j < me->vertices.size(); j++) {
            const Vec &v = me->vertices[j];
            std::cout << "  v " << v.x_ << "," << v.y_  << "," << v.z_ << std::endl;
        }
        std::cout << " normals:" << me->normals.size() << std::endl;
        for(int j = 0; j < me->normals.size(); j++) {
            const Vec &v = me->normals[j];
            std::cout << "  vn " << v.x_ << "," << v.y_  << "," << v.z_ << std::endl;
        }
        std::cout << " attr[0](uv):" << me->attributes[0].size() << std::endl;
        for(int j = 0; j < me->attributes[0].size(); j++) {
            const Vec &v = me->attributes[0][j];
            std::cout << "  vt " << v.x_ << "," << v.y_  << "," << v.z_ << std::endl;
        }
        std::cout << " faces(tri):" << me->faces.size() << std::endl;
        for(int j = 0; j < me->faces.size(); j++) {
            const Mesh::Face &f = me->faces[j];
            std::cout << "  f " << f.v0 << "," << f.v1  << "," << f.v2 << std::endl;
        }
    }
    */
}

void r1h::setupAABBTestScene(Scene &scene, const double aspect) {
    scene.BackgroundColor = Color(1.0, 1.0, 1.0);
    scene.objects.reserve(10);
    
    EduptMaterial *mat;
    AABBGeometry *geom;
    Object *obj;
    
    // floor
    geom = new AABBGeometry(Vec(-100.0, -100.0, -100.0), Vec(100.0, 0.0, 100.0));
    obj = new Object(geom);
    mat = new EduptMaterial(Color(0.5), Color(0.0), REFLECTION_TYPE_DIFFUSE);
    geom->addMaterial(mat);
    mat->release();
    geom->release();
    scene.addObject(obj);
    
    Random rnd(2345);
    for(int i = -4; i < 4; i++) {
        double x = i * 10.0;
        double z = (rnd.next01() - 0.5) * 10.0;
        geom = new AABBGeometry(Vec(x-4.0, 0.0, z-5.0), Vec(x+4.0, 8.0 + rnd.next01(), z+5.0));
        obj = new Object(geom);
        mat = new EduptMaterial(Color(rnd.next01(), rnd.next01(), rnd.next01()), Color(0.0), REFLECTION_TYPE_DIFFUSE);
        geom->addMaterial(mat);
        mat->release();
        geom->release();
        scene.addObject(obj);
    }
    
    Camera *camera = new Camera();
    camera->setLookat(
        Vec(0.0, 20.0, 40.0),
        Vec(0.0, 0.0, 0.0),
        Vec(0.0, 1.0, 0.0)
    );
    camera->setFocal(40.0, 30.0 * aspect);
    camera->setAspectRatio(aspect);
    scene.setCamera(camera);
    camera->release();
}

void r1h::setupManyTrisObjScene(Scene &scene, const double aspect) {
    scene.BackgroundColor = Color(0.8);
    
    //TestObjImport importer(&scene, "manypoly0.obj"); // low
    //TestObjImport importer(&scene, "manypoly1.obj"); // high
    TestObjImport importer(&scene, "suzanne2.obj"); // higher
    importer.setBasePath("../bin/model");
    importer.load();
    
    // camera for cornel box
    Camera *camera = new Camera();
    camera->setLookat(
        Vec(0.0, 0.0, 5.0),
        Vec(0.0, 0.0, 0.0),
        Vec(0.0, 1.0, 0.0)
    );
    camera->setFocal(35.0, 32.0);
    camera->setAspectRatio(aspect);
    scene.setCamera(camera);
    camera->release();
}

void r1h::setupEduptScene(Scene &scene, const double aspect) {
    scene.BackgroundColor = Color(0.0, 0.0, 0.0);
    
    scene.objects.reserve(10);
    
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec( 1e5 + 1.0, 40.8, 81.6),    Color(), Color(0.75, 0.25, 0.25), REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(-1e5 + 99.0, 40.8, 81.6),   Color(), Color(0.25, 0.25, 0.75), REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(50.0, 40.8, 1e5),           Color(), Color(0.75, 0.75, 0.75), REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(50.0, 40.8, 1e5 + 250.0),   Color(), Color(),                 REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(50.0, 1e5, 81.6),           Color(), Color(0.75, 0.75, 0.75), REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(1e5, Vec(50.0, 1e5 + 81.6, 81.6),    Color(), Color(0.75, 0.75, 0.75), REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(20.0, Vec(65.0, 20.0, 20.0),         Color(), Color(0.25, 0.75, 0.25), REFLECTION_TYPE_DIFFUSE))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(16.5, Vec(27.0, 16.5, 47.0),         Color(), Color(0.99, 0.99, 0.99), REFLECTION_TYPE_SPECULAR))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(16.5, Vec(77.0, 16.5, 78.0),         Color(), Color(0.99, 0.99, 0.99), REFLECTION_TYPE_REFRACTION))
    ));
    scene.objects.push_back(new Object(
        RCO::rc0(new Sphere(15.0, Vec(50.0, 90.0, 81.6),         Color(36.0, 36.0, 36.0), Color(), REFLECTION_TYPE_DIFFUSE))
    ));
    
    //+++++
    // triangle
    Mesh *mesh = new Mesh(3, 1);
    mesh->addMaterial(RCO::rc0(new EduptMaterial(Color(0.25, 0.75, 0.25), Color(), REFLECTION_TYPE_DIFFUSE)));
    mesh->addVertexWithAttrs(Vec(11.0, 38.0, 81.5), Vec(0.0, 1.0, 0.0));
    mesh->addVertexWithAttrs(Vec(24.0, 16.0, 100.0), Vec(0.0, 1.0, 0.0));
    mesh->addVertexWithAttrs(Vec(38.0, 33.5, 70.0), Vec(0.0, 1.0, 0.0));
    mesh->addVertexWithAttrs(Vec(33.0, 5.5, 73.0), Vec(0.0, 1.0, 0.0));
    mesh->addFace(0, 1, 2);
    mesh->addFace(3, 1, 2);
    mesh->calcSmoothNormals();
    
    scene.objects.push_back(new Object(mesh));
    mesh->release();
    //+++++
    
    // camera
    scene.setCamera(RCO::rc0<Camera>(new Camera()));
    scene.camera->setLookat(
        Vec(50.0, 52.0, 220.0),
        Vec(50.0, 52.0 - 0.04, 220.0 - 1.0),
        Vec(0.0, 1.0, 0.0)
    );
    scene.camera->setFocal(40.0, 30.0 * aspect);
    scene.camera->setAspectRatio(aspect);
}
