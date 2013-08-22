#ifndef R1H_OBJLOADER_H
#define R1H_OBJLOADER_H

#include <string>
#include <map>

namespace r1h {

class WavefrontObj {
    std::string basepath;
    std::string filepath;
    
public:
    enum ParameterType {
        // obj
        OBJ_unknown,
        OBJ_v   = 128,
        OBJ_vn,
        OBJ_vt,
        OBJ_o,
        OBJ_g,
        OBJ_s,
        OBJ_f,
        OBJ_mtllib,
        OBJ_usemtl,
        // matl
        MTL_newmtl,
        MTL_Ns,
        MTL_Ka,
        MTL_Kd,
        MTL_Ks,
        MTL_Ni,
        MTL_d,
        MTL_illum,
        MTL_map_Kd
    };
    
    struct FaceInfo {
        int v, vt, vn;
    };
    
    WavefrontObj(const char *path): filepath(path) {}
    virtual ~WavefrontObj(){}
    
    virtual void setBasePath(const char *path) {
        basepath = path;
        if(basepath[basepath.length() - 1] != '/') {
            basepath.push_back('/');
        }
    }
    virtual void load();
    
    // implement below
    // v, vn, vt, Ns, Ka, Kd, Ks, Ni, d
    virtual void foundVector(const ParameterType pt, const double x, const double y, const double z) = 0;
    // mtllib, o, s, usemtl, newmtl, MTL_map_Kd
    virtual void foundString(const ParameterType pt, const std::string &str) = 0;
    // illum
    virtual void foundInteger(const ParameterType pt, const int i) = 0;
    // f
    virtual void foundFace(const ParameterType pt, const std::vector<FaceInfo> &fids) = 0;
    
    // mtl
    virtual void importMTL(const char *filename); // optional
    
    // endof .mtl or .obj
    virtual void endFile() = 0;
    
private:
    std::map<std::string, ParameterType> ptMap;
    void loadFile(const char *fpath);
};
}
#endif