#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <map>
#include <vector>
#include "wavefrontobj.h"

using namespace r1h;

/*
// vector
case OBJ_v:
    break;
case OBJ_vn:
    break;
case OBJ_vt:
    break;
case MTL_Ns:
    break;
case MTL_Ka:
    break;
case MTL_Kd:
    break;
case MTL_Ks:
    break;
case MTL_Ni:
    break;
case MTL_d:
    break;
default:

// string
case OBJ_o:
    break;
case OBJ_g:
    break;
case OBJ_s:
    break;
case OBJ_f:
    break;
case OBJ_mtllib:
    break;
case OBJ_usemtl:
    break;
case MTL_map_Kd:
    break;

// int
case MTL_illum:
        break;
*/

void WavefrontObj::load() {
    loadFile(filepath.c_str());
}

void WavefrontObj::loadFile(const char *fpath) {
    std::ifstream *fsp = new std::ifstream(fpath);
    
    if(!fsp->is_open()) {
        //std::cout << fpath << " couldn't open" << std::endl;
        delete fsp;
        
        if(basepath.length() <= 0) {
            std::cout << fpath << " couldn't open. no base path." << std::endl;
            return;
        }
        
        std::string fullpath = basepath + fpath;
        fsp = new std::ifstream(fullpath.c_str());
        if(!fsp->is_open()) {
            std::cout << fpath << " or " << fullpath << " couldn't open" << std::endl;
            delete fsp;
            return;
        }
    }
    
    //std::map<std::string, ParameterType> ptMap;
    if(ptMap.size() <= 0) {
        ptMap["v"] =        OBJ_v;
        ptMap["vn"] =       OBJ_vn;
        ptMap["vt"] =       OBJ_vt;
        ptMap["o"] =        OBJ_o;
        ptMap["g"] =        OBJ_g;
        ptMap["s"] =        OBJ_s;
        ptMap["f"] =        OBJ_f;
        ptMap["mtllib"] =   OBJ_mtllib;
        ptMap["usemtl"] =   OBJ_usemtl;
        ptMap["newmtl"] =   MTL_newmtl;
        ptMap["Ns"] =       MTL_Ns;
        ptMap["Ka"] =       MTL_Ka;
        ptMap["Kd"] =       MTL_Kd;
        ptMap["Ks"] =       MTL_Ks;
        ptMap["Ni"] =       MTL_Ni;
        ptMap["d"] =        MTL_d;
        ptMap["illum"] =    MTL_illum;
        ptMap["map_Kd"] =   MTL_map_Kd;
    }
    
    const int BufMmax = 1024;
    char *buf = new char[1024];
    
    std::vector<FaceInfo> finfovec;
    finfovec.reserve(8);
    
    int lineCount = 0;
    while(!fsp->eof()) {
        ++lineCount;
        
        fsp->getline(buf, BufMmax);
        if(fsp->gcount() <= 0) {
            //std::cout << lineCount << ": empty line. continue" << std::endl;
            continue;
        }
        
        std::istringstream sstrm(buf);
        if(!sstrm) {
            std::cout << "line string stream couldn't open" << std::endl;
            continue;
        }
        
        while(!sstrm.eof()) {
            std::string tmps;
            sstrm >> tmps;
            
            if(tmps[0] == '#' || tmps[0] == '\0') {
                //std::cout << lineCount << " skip line" << std::endl;
                break;
            }
            
            ParameterType pt = ptMap[tmps];
            
            switch(pt) {
                case OBJ_v:
                case OBJ_vn:
                case OBJ_vt:
                case MTL_Ns:
                case MTL_Ka:
                case MTL_Kd:
                case MTL_Ks:
                case MTL_Ni:
                case MTL_d: {
                        // vector
                        double vary[3] = {0.0, 0.0, 0.0};
                        int i = 0;
                        for(; i < 3 && !sstrm.eof(); i++) {
                            sstrm >> vary[i];
                        }
                        this->foundVector(pt, vary[0], vary[1], vary[2]);
                    }
                    break;
                case OBJ_o:
                case OBJ_g:
                case OBJ_s:
                case OBJ_mtllib:
                case OBJ_usemtl:
                case MTL_newmtl:
                case MTL_map_Kd: {
                        // string
                        std::string s;
                        sstrm >> s;
                        this->foundString(pt, s);
                    }
                    break;
                case MTL_illum: {
                        // int
                        int i;
                        sstrm >> i;
                        this->foundInteger(pt, i);
                    }
                    break;
                case OBJ_f: {
                        // face
                        finfovec.clear();
                        
                        while(!sstrm.eof()) {
                            FaceInfo fcinfo;
                            std::string fistr;
                            sstrm >> fistr;
                            //std::cout << "'" << fistr << "' : ";
                            
                            std::istringstream facestrm(fistr);
                            facestrm >> fcinfo.v;
                            if(facestrm.eof()) {
                                fcinfo.vt = fcinfo.v;
                                fcinfo.vn = fcinfo.v;
                            } else {
                                char purgec;
                                facestrm >> purgec;
                                facestrm >> fcinfo.vt;
                                if(facestrm.fail()) {
                                    fcinfo.vt = 0;
                                    facestrm.clear();
                                }
                                facestrm >> purgec;
                                facestrm >> fcinfo.vn;
                            }
                            finfovec.push_back(fcinfo);
                        }
                        this->foundFace(pt, finfovec);
                    }
                    break;
                default:
                    std::cout << lineCount << " Wavefront .obj unknown line : " << tmps << std::endl;
            }
        }
    }
    
    delete [] buf;
    
    fsp->close();
    delete fsp;
    
    this->endFile();
}

void WavefrontObj::importMTL(const char *filename) {
    loadFile(filename);
}

