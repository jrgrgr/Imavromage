#pragma once

#include <cstdlib>
#include <fstream>
#include <unordered_map>
#include <vector>

#include <ivmg/codecs/decoder.hpp>
#include "macros.hpp"
#include "utils.hpp"

using namespace ivmg::types;


namespace ivmg {


enum class ChunkType : u32 {
    IHDR = 0x49484452,
    PLTE = 0x504C5445,
    IDAT = 0x49444154,
    IEND = 0x49454E44,
};


struct ChunkPNG {
    u32 length;
    ChunkType type;
    u32 crc;
    std::span<u8> data;
};



enum class PNG_COLOR_TYPE : u8 {
    GSC = 0,
    RGB = 2,
    IDX = 3,
    GSCA = 4,
    RGBA = 6
};

const std::unordered_map<PNG_COLOR_TYPE, u8> channel_nb {
    { PNG_COLOR_TYPE::GSC, 1 },
    { PNG_COLOR_TYPE::RGB, 3 },
    { PNG_COLOR_TYPE::GSCA, 2 },
    { PNG_COLOR_TYPE::RGBA, 4 }
};


enum class PNG_FILT_TYPE : u8 {
    NONE = 0,
    SUB = 1,
    UP = 2,
    AVG = 3,
    PAETH = 4
};

constexpr u8 magic_length = 8;
constexpr u8 magic[magic_length] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };


class PNG_Decoder : public Decoder {

private:
    size_t bpp;
    u32 width;
    u32 height;
    u8 bit_depth;
    PNG_COLOR_TYPE color_type;
    u8 compression_method;
    u8 filter_method;
    u8 interlace_method;
    std::vector<u8> compressed_data;
    std::vector<u8> inflated_data;

public:
    inline PNG_Decoder(): Decoder() {};
    bool can_decode(std::ifstream& filestream) const override;
    Image decode(std::ifstream& filestream) override;

private:
    ChunkPNG read_chunk(std::vector<u8>& file_buffer, size_t &read_idx);
    void decode_ihdr(std::span<u8> data);
    Image decode_png(std::vector<u8>& file_buffer);
    u8 paeth_predictor(u8 a, u8 b, u8 c);
    std::optional<std::span<const u8>> get_scanline(std::span<const u8>& data, size_t scanline_size);
};



}
