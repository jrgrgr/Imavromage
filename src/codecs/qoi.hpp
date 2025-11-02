#pragma once

#include <ivmg/core/image.hpp>
#include "utils.hpp"
#include <cassert>
#include <filesystem>


namespace ivmg {

using namespace types;

struct qoi_color_t { 
    u8 r, g, b, a; 
    bool operator==(const qoi_color_t& other) const {
        return (r == other.r) && (g == other.g) && (b == other.b) && (a == other.a);
    }
};

struct qoi_diff_t { i8 r, g, b; };

enum class QOI_COLORSPACE: u8 {
    SRGB = 0,
    LINEAR = 1
};

// QOI chunks initialized. Tags are set but without data
const u8 QOI_OP_RGB   =  0xFE;     // 8 bits tag
const u8 QOI_OP_RGBA  =  0xFF;     // 8 bits tag
const u8 QOI_OP_LUMA  =  0x80;     // 2 bits tag
const u8 QOI_OP_INDEX =  0x00;     // 2 bits tag
const u8 QOI_OP_DIFF  =  0x40;     // 2 bits tag
const u8 QOI_OP_RUN   =  0xC0;     // 2 bits tag


const u8 QOI_MASK_2   =  0xC0;

struct qoi_header {
    u8 magic[4] = {'q', 'o', 'i', 'f'};
    u32 width;             // image width in pixels (BE)
    u32 height;            // image height in pixels (BE)
    u8 channels;          // 3 = RGB, 4 = RGBA
    QOI_COLORSPACE colorspace;        // 0 = sRGB with linear alpha | 1 = all channels linear
} __attribute__((packed));


void encode_qoi(const Image& img, const std::filesystem::path& outfile);
constexpr qoi_diff_t QOI_COLOR_DIFF(const qoi_color_t& c1, const qoi_color_t& c2);
constexpr size_t QOI_PIXEL_HASH(const qoi_color_t& c);




}
