#pragma once

#include <ivmg/codecs/decoder.hpp>

#include <cstdlib>
#include <unordered_map>
#include <vector>


namespace ivmg {


enum class ChunkType : uint32_t {
    IHDR = 0x49484452,
    PLTE = 0x504C5445,
    IDAT = 0x49444154,
    IEND = 0x49454E44,
};


struct ChunkPNG {
    uint32_t length;
    ChunkType type;
    uint32_t crc;
    std::span<uint8_t> data;
};



enum class PNG_COLOR_TYPE : uint8_t {
    GSC = 0,
    RGB = 2,
    IDX = 3,
    GSCA = 4,
    RGBA = 6
};

const std::unordered_map<PNG_COLOR_TYPE, uint8_t> channel_nb {
    { PNG_COLOR_TYPE::GSC, 1 },
    { PNG_COLOR_TYPE::RGB, 3 },
    { PNG_COLOR_TYPE::GSCA, 2 },
    { PNG_COLOR_TYPE::RGBA, 4 }
};


enum class PNG_FILT_TYPE : uint8_t {
    NONE = 0,
    SUB = 1,
    UP = 2,
    AVG = 3,
    PAETH = 4
};

constexpr uint8_t magic_length = 8;
constexpr uint8_t magic[magic_length] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };


class PNG_Decoder : public Decoder {

private:
    size_t bpp;
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    PNG_COLOR_TYPE color_type;
    uint8_t compression_method;
    uint8_t filter_method;
    uint8_t interlace_method;
    std::vector<uint8_t> compressed_data;
    std::vector<uint8_t> inflated_data;

public:
    PNG_Decoder() = default;
    bool can_decode(std::ifstream& filestream) const override;
    Image decode(std::ifstream& filestream) override;

private:
    ChunkPNG read_chunk(std::vector<uint8_t>& file_buffer, size_t &read_idx);
    void decode_ihdr(std::span<uint8_t> data);
    Image decode_png(std::vector<uint8_t>& file_buffer);
    uint8_t paeth_predictor(uint8_t a, uint8_t b, uint8_t c);
    std::optional<std::span<const uint8_t>> get_scanline(std::span<const uint8_t>& data, size_t scanline_size);
};



}
