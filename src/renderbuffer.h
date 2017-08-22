#ifndef R1H_RENDER_BUFFER_H
#define R1H_RENDER_BUFFER_H

#include <fstream>
#include "Color.h"

namespace r1h {

class RenderBuffer
{
    Color *buffer;
    int width;
    int height;
    unsigned long sampleCount;
    
    inline int componentToInt(double c, const double gamma=2.2) {
        if(c < 0.0) {
            c = 0.0;
        } else if(c > 1.0) {
            c = 1.0;
        }
        return int(pow(c, 1.0 / gamma) * 255 + 0.5);
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
    
public:
    RenderBuffer(const int w, const int h): buffer(0), width(w), height(h), sampleCount(0) {
        buffer = new Color[w * h];
    }
    ~RenderBuffer() {
        delete [] buffer;
    }
    
    void clear() {
        for(int i = 0; i < width * height; i++) {
            buffer[i].set(0.0, 0.0, 0.0);
        }
        sampleCount = 0;
    }
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    unsigned long getCurrentSampleCount() const { return sampleCount; }
    
    void incrementSampleCount(const unsigned int i=1) {
        sampleCount += i;
    }
    
    void accumulate(const int x, const int y, const double r, const double g, const double b) {
        Color &col = buffer[x + y * width];
        col.r += r;
        col.g += g;
        col.b += b;
    }
    void accumulate(const int x, const int y, const Color &cmp) {
        accumulate(x, y, cmp.r, cmp.g, cmp.b);
    }
    
    // don't forget delete!
    unsigned char* createRGB8(const double gamma=2.2) {
        unsigned char *retbuf = new unsigned char[width * height * 3];
        
        for(int i = 0; i < width * height; i++) {
            Color rgbcomp = buffer[i] / ColorRGBType(sampleCount);
            unsigned char *rgb8ptr = &retbuf[i * 3];
            
            rgb8ptr[0] = componentToInt(rgbcomp.r, gamma);
            rgb8ptr[1] = componentToInt(rgbcomp.g, gamma);
            rgb8ptr[2] = componentToInt(rgbcomp.b, gamma);
        }
        
        return retbuf;
    }
        
    void exportBMP(const char *filepath) {
        
        std::ofstream ofs(filepath, std::ios::out | std::ios::binary);
        
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
        const double Gamma = 2.2;
        const ColorRGBType divsample = 1.0 / sampleCount;
        for(int y = height - 1; y >= 0; y--) {
            for(int x = 0; x < width; x++) {
                const Color c = buffer[y * width + x] * divsample;
                ofs.put(componentToInt(c.b, Gamma));
                ofs.put(componentToInt(c.g, Gamma));
                ofs.put(componentToInt(c.r, Gamma));
            }
        }
        ofs.close();
        
        //std::cerr << filepath << " saved" << std::endl;
    }
};


}
#endif