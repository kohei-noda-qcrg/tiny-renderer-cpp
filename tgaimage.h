#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <cstdint>
#include <fstream>
#include <vector>

#pragma pack(push, 1)
struct TGA_Header {
    char  idlength;
    char  colormaptype;
    char  datatypecode;
    short colormaporigin;
    short colormaplength;
    char  colormapdepth;
    short x_origin;
    short y_origin;
    short width;
    short height;
    char  bitsperpixel;
    char  imagedescriptor;
};
#pragma pack(pop)

struct TGAColor {
    union {
        struct {
            uint8_t b, g, r, a;
        };
        uint8_t      raw[4];
        unsigned int val;
    };
    int bytespp;

    TGAColor() : val(0), bytespp(1) {
    }

    TGAColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A) : b(B), g(G), r(R), a(A), bytespp(4) {
    }

    TGAColor(int v, int bpp) : val(v), bytespp(bpp) {
    }

    TGAColor(const TGAColor& c) : val(c.val), bytespp(c.bytespp) {
    }

    TGAColor(const uint8_t p[4], int bpp) : val(0), bytespp(bpp) {
        for(int i = 0; i < bpp; i++) {
            raw[i] = p[i];
        }
    }

    TGAColor& operator=(const TGAColor& c) {
        if(this != &c) {
            bytespp = c.bytespp;
            val     = c.val;
        }
        return *this;
    }

    TGAColor operator*(const double intensity) {
        auto ret = *this;
        ret.r *= intensity;
        ret.g *= intensity;
        ret.b *= intensity;
        ret.a *= intensity;
        return ret;
    }
};

class TGAImage {
  public:
    enum Format {
        GRAYSCALE = 1,
        RGB       = 3,
        RGBA      = 4
    };

    TGAImage();
    TGAImage(const size_t w, const size_t h, const Format bpp);
    TGAImage(const TGAImage& img);
    bool     read_tga_file(const std::string filename);
    bool     write_tga_file(const std::string filename, const bool rle = true);
    bool     flip_horizontally();
    bool     flip_vertically();
    bool     scale(int w, int h);
    TGAColor get(int x, int y) const;
    bool     set(int x, int y, TGAColor c);
    ~TGAImage() = default;
    TGAImage& operator=(const TGAImage& img);
    size_t    get_width() const;
    size_t    get_height() const;
    Format    get_format() const;
    uint8_t*  buffer();
    void      fill(const uint8_t v);
    void      clear();

  private:
    std::vector<uint8_t> data;
    size_t               width;
    size_t               height;
    Format               format;

    bool load_rle_data(std::ifstream& in);
    bool unload_rle_data(std::ofstream& out);
};

#endif //__IMAGE_H__
