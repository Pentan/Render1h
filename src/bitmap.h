#ifndef R1H_BITMAP_H
#define R1H_BITMAP_H

#include <fstream>
//#include "material.h"
#include "color.h"

namespace r1h {

inline double clamp(double x) {
    if(x < 0.0) return 0.0;
    if(x > 1.0) return 1.0;
    return x;
}

inline int to_int(double x, double gamma) {
    return int(pow(clamp(x), 1.0/gamma) * 255 + 0.5);
}

inline void put16(const unsigned short s, std::ofstream &ofs) {
    ofs.put(s & 0xff);
    ofs.put((s >> 8) & 0xff);
}

inline void put32(const unsigned long l, std::ofstream &ofs) {
    ofs.put(l & 0xff);
    ofs.put((l >> 8) & 0xff);
    ofs.put((l >> 16) & 0xff);
    ofs.put((l >> 24) & 0xff);
}

void save_bmp_file(const std::string &filename, const Color *image, const int width, const int height, const double gamma=2.2) {
    std::ofstream ofs(filename.c_str(), std::ios::out | std::ios::binary);
    
    // header
    // signature
    ofs << "BM";
    // total length ([RGB datas] + [BMP header(14)] + [BMP image header(40)])
    put32(width * height * 3 + 14 + 40, ofs);
    // reserved
    put16(0, ofs);
    put16(0, ofs);
    // offset to image
    put32(14 + 40, ofs);
    
    // image header
    // header size
    put32(40, ofs);
    // image size
    put32(width, ofs);
    put32(height, ofs);
    // bit lanes
    put16(1, ofs);
    // color bits
    put16(24, ofs);
    // compression (0:none)
    put32(0, ofs);
    // bitmap length
    put32(width * height * 3, ofs);
    // dot per meter (72dpi)
    put32(2835, ofs);
    put32(2835, ofs);
    // CLUT info
    put32(0, ofs);
    put32(0, ofs);
    
    // bitmap
    for(int y = height - 1; y >= 0; y--) {
        for(int x = 0; x < width; x++) {
            const Color &c = image[y * width + x];
            ofs.put(to_int(c.z_, gamma));
            ofs.put(to_int(c.y_, gamma));
            ofs.put(to_int(c.x_, gamma));
        }
    }
    
    ofs.close();
    
    std::cerr << filename << " saved" << std::endl;
}


}

#endif
