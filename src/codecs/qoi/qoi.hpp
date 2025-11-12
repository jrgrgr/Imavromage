#pragma once

#include "ivmg/codecs/encoder.hpp"
#include <cstddef>
#include <filesystem>

namespace ivmg {

class Image;

struct qoi_color_t {
    uint8_t r, g, b, a;
    bool operator==(const qoi_color_t& other) const {
        return (r == other.r) && (g == other.g) && (b == other.b) && (a == other.a);
    }
};

struct qoi_diff_t { int8_t r, g, b; };

enum class QOI_COLORSPACE: uint8_t {
    SRGB = 0,
    LINEAR = 1
};

// QOI chunks initialized. Tags are set but without data
const uint8_t QOI_OP_RGB   =  0xFE;     // 8 bits tag
const uint8_t QOI_OP_RGBA  =  0xFF;     // 8 bits tag
const uint8_t QOI_OP_LUMA  =  0x80;     // 2 bits tag
const uint8_t QOI_OP_INDEX =  0x00;     // 2 bits tag
const uint8_t QOI_OP_DIFF  =  0x40;     // 2 bits tag
const uint8_t QOI_OP_RUN   =  0xC0;     // 2 bits tag


const uint8_t QOI_MASK_2   =  0xC0;

struct qoi_header {
    uint8_t magic[4] = {'q', 'o', 'i', 'f'};
    uint32_t width;             // image width in pixels (BE)
    uint32_t height;            // image height in pixels (BE)
    uint8_t channels;          // 3 = RGB, 4 = RGBA
    QOI_COLORSPACE colorspace;        // 0 = sRGB with linear alpha | 1 = all channels linear
} __attribute__((packed));


void encode_qoi(const Image& img, const std::filesystem::path& outfile);
constexpr qoi_diff_t QOI_COLOR_DIFF(const qoi_color_t& c1, const qoi_color_t& c2);
constexpr size_t QOI_PIXEL_HASH(const qoi_color_t& c);





class QoiEncoder: public Encoder {
private:
	// File metadata
	static constexpr size_t hdr_size = 14;
	static constexpr uint32_t magic = 0x716F6966;
	static constexpr std::array<char, 8> end_marker {0,0,0,0,0,0,0,1};
	uint8_t channels = 4;
	QOI_COLORSPACE colorspace = QOI_COLORSPACE::SRGB;

	std::array<qoi_color_t, 64> color_cache {};
	std::vector<uint8_t> encoded_data;
	qoi_color_t prev_pxl { 0, 0, 0, 255 };
	uint16_t run = 0;
	size_t ptr = 0;

	// Helpers
	static uint16_t hash_pixel(const qoi_color_t& c);
	static qoi_diff_t color_diff(const qoi_color_t& c1, const qoi_color_t& c2);

public:
	QoiEncoder() = default;
	std::vector<uint8_t> encode(const Image& img) override;
};





}
