#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string.h>

#include "tgaimage.h"

TGAImage::TGAImage() : data{}, width(0), height(0), format(RGB) {}
TGAImage::TGAImage(const size_t width, const size_t height, const Format format) : data(width * height * format, 0), width(width), height(height), format(format) {}

TGAImage::TGAImage(const TGAImage& img) {
    data   = img.data;
    width  = img.width;
    height = img.height;
    format = img.format;
}

TGAImage& TGAImage::operator=(const TGAImage& img) {
    if(this != &img) {
        data   = img.data;
        width  = img.width;
        height = img.height;
        format = img.format;
    }
    return *this;
}

bool TGAImage::read_tga_file(const std::string filename) {
    auto in = std::ifstream();
    in.open(filename, std::ios::binary);
    if(!in.is_open()) {
        std::cerr << "can't open file " << filename << "\n";
        in.close();
        return false;
    }
    auto header = TGA_Header{};
    in.read((char*)&header, sizeof(header));
    if(!in.good()) {
        in.close();
        std::cerr << "an error occured while reading the header\n";
        return false;
    }
    width  = header.width;
    height = header.height;
    format = Format(header.bitsperpixel >> 3);
    if(width <= 0 || height <= 0 || (format != GRAYSCALE && format != RGB && format != RGBA)) {
        in.close();
        std::cerr << "bad bpp (or width/height) value\n";
        return false;
    }
    const auto nbytes = width * height * format;
    data.resize(nbytes);
    if(3 == header.datatypecode || 2 == header.datatypecode) {
        in.read((char*)data.data(), nbytes);
        if(!in.good()) {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    } else if(10 == header.datatypecode || 11 == header.datatypecode) {
        if(!load_rle_data(in)) {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    } else {
        in.close();
        std::cerr << "unknown file format " << (int)header.datatypecode << "\n";
        return false;
    }
    if(!(header.imagedescriptor & 0x20)) {
        flip_vertically();
    }
    if(header.imagedescriptor & 0x10) {
        flip_horizontally();
    }
    std::cerr << width << "x" << height << "/" << format * 8 << "\n";
    in.close();
    return true;
}

bool TGAImage::load_rle_data(std::ifstream& in) {
    auto pixelcount   = width * height;
    auto currentpixel = 0ul;
    auto currentbyte  = 0ul;
    auto colorbuffer  = TGAColor();
    do {
        auto chunkheader = in.get();
        if(!in.good()) {
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
        if(chunkheader < 128) {
            chunkheader++;
            for(auto i = 0; i < chunkheader; i++) {
                in.read((char*)colorbuffer.raw, format);
                if(!in.good()) {
                    std::cerr << "an error occured while reading the header\n";
                    return false;
                }
                for(auto t = 0; t < format; t++)
                    data[currentbyte++] = colorbuffer.raw[t];
                currentpixel++;
                if(currentpixel > pixelcount) {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        } else {
            chunkheader -= 127;
            in.read((char*)colorbuffer.raw, format);
            if(!in.good()) {
                std::cerr << "an error occured while reading the header\n";
                return false;
            }
            for(auto i = 0; i < chunkheader; i++) {
                for(int t = 0; t < format; t++)
                    data[currentbyte++] = colorbuffer.raw[t];
                currentpixel++;
                if(currentpixel > pixelcount) {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
    } while(currentpixel < pixelcount);
    return true;
}

bool TGAImage::write_tga_file(const std::string filename, const bool rle) {

    const auto developer_area_ref = std::array<uint8_t, 4>{0, 0, 0, 0};
    const auto extension_area_ref = std::array<uint8_t, 4>{0, 0, 0, 0};
    const auto footer             = std::string("TRUEVISION-XFILE.");
    auto       out                = std::ofstream();
    out.open(filename, std::ios::binary);
    if(!out.is_open()) {
        std::cerr << "can't open file " << filename << "\n";
        out.close();
        return false;
    }
    TGA_Header header;
    memset((void*)&header, 0, sizeof(header));
    header.bitsperpixel    = format << 3;
    header.width           = width;
    header.height          = height;
    header.datatypecode    = (format == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
    header.imagedescriptor = 0x00; // top-left origin
    out.write((char*)&header, sizeof(header));
    if(!out.good()) {
        out.close();
        std::cerr << "can't dump the tga file\n";
        return false;
    }
    if(!rle) {
        out.write((char*)data.data(), width * height * format);
        if(!out.good()) {
            std::cerr << "can't unload raw data\n";
            out.close();
            return false;
        }
    } else {
        if(!unload_rle_data(out)) {
            out.close();
            std::cerr << "can't unload rle data\n";
            return false;
        }
    }
    out.write(reinterpret_cast<const char*>(developer_area_ref.data()), developer_area_ref.size());
    if(!out.good()) {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write(reinterpret_cast<const char*>(extension_area_ref.data()), extension_area_ref.size());
    if(!out.good()) {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write(footer.c_str(), footer.size() + 1); // +1 for NULL
    if(!out.good()) {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.close();
    return true;
}

// TODO: it is not necessary to break a raw chunk for two equal pixels (for the matter of the resulting size)
bool TGAImage::unload_rle_data(std::ofstream& out) {
    const auto max_chunk_length = uint8_t(128);
    const auto npixels          = width * height;
    auto       curpix           = 0uz;
    while(curpix < npixels) {
        auto chunkstart = curpix * format;
        auto curbyte    = curpix * format;
        auto run_length = uint8_t(1);
        auto raw        = true;
        while(curpix + run_length < npixels && run_length < max_chunk_length) {
            auto succ_eq = true;
            for(auto t = 0uz; succ_eq && t < format; t++) {
                succ_eq = (data[curbyte + t] == data[curbyte + t + format]);
            }
            curbyte += format;
            if(1 == run_length) {
                raw = !succ_eq;
            }
            if(raw && succ_eq) {
                run_length--;
                break;
            }
            if(!raw && !succ_eq) {
                break;
            }
            run_length++;
        }
        curpix += run_length;
        out.put(raw ? run_length - 1 : run_length + 127);
        if(!out.good()) {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
        out.write((char*)(data.data() + chunkstart), (raw ? run_length * format : format));
        if(!out.good()) {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
    }
    return true;
}

TGAColor TGAImage::get(int x, int y) const {
    if(data.empty() || x < 0 || y < 0 || x >= width || y >= height) {
        return TGAColor();
    }
    return TGAColor(data.data() + (x + y * width) * format, format);
}

bool TGAImage::set(int x, int y, TGAColor c) {
    if(data.empty() || x < 0 || y < 0 || x >= width || y >= height) {
        return false;
    }
    memcpy(data.data() + (x + y * width) * format, c.raw, format);
    return true;
}

TGAImage::Format TGAImage::get_format() const {
    return format;
}

size_t TGAImage::get_width() const {
    return width;
}

size_t TGAImage::get_height() const {
    return height;
}

bool TGAImage::flip_horizontally() {
    if(data.empty()) return false;
    const auto bytes_per_line = width * format;

    const auto half = width >> 1;
    for(auto j = 0uz; j < height; j++) {
        auto* start = &data[j * bytes_per_line];
        for(auto i = 0uz; i < half; i++) {
            auto* left  = start + i * format;
            auto* right = start + (width - 1 - i) * format;
            std::swap_ranges(left, left + format, right);
        }
    }
    return true;
}

bool TGAImage::flip_vertically() {
    if(data.empty()) return false;
    const auto bytes_per_line = width * format;

    const auto half = height >> 1;
    for(auto j = 0uz; j < half; j++) {
        const auto l1 = j * bytes_per_line;
        const auto l2 = (height - 1 - j) * bytes_per_line;
        std::swap_ranges(data.begin() + l1,
                         data.begin() + l1 + bytes_per_line,
                         data.begin() + l2);
    }
    return true;
}

unsigned char* TGAImage::buffer() {
    return data.data();
}

void TGAImage::clear() {
    data.clear();
}

bool TGAImage::scale(int w, int h) {
    if(w <= 0 || h <= 0 || data.empty()) return false;
    auto tdata      = std::vector<uint8_t>(w * h * format, 0);
    auto nscanline  = 0;
    auto oscanline  = 0;
    auto erry       = 0;
    auto nlinebytes = size_t(w * format);
    auto olinebytes = width * format;
    for(auto j = 0uz; j < height; j++) {
        auto errx = width - w;
        auto nx   = -format;
        auto ox   = -format;
        for(auto i = 0uz; i < width; i++) {
            ox += format;
            errx += w;
            while(errx >= width) {
                errx -= width;
                nx += format;
                memcpy(tdata.data() + nscanline + nx, data.data() + oscanline + ox, format);
            }
        }
        erry += h;
        oscanline += olinebytes;
        while(erry >= int(height)) {
            if(erry >= int(height << 1)) // it means we jump over a scanline
                memcpy(tdata.data() + nscanline + nlinebytes, tdata.data() + nscanline, nlinebytes);
            erry -= height;
            nscanline += nlinebytes;
        }
    }
    data   = tdata;
    width  = w;
    height = h;
    return true;
}
