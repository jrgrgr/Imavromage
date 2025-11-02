#include "logger.hpp"
#include "png.hpp"
#include <chrono>
#include <cstring>
#include <libdeflate.h>
#include <ivmg/core/image.hpp>
#include <optional>

namespace ivmg {

bool PNG_Decoder::can_decode(std::ifstream& filestream) const {
    auto startpos = filestream.tellg();
    char buffer[magic_length] = {0};
    if (filestream.read(buffer, magic_length) && !filestream.fail() && std::memcmp(buffer, magic, magic_length) == 0) {
        filestream.seekg(startpos);
        return true;
    }
    return false;
}


Image PNG_Decoder::decode(std::ifstream& filestream) {
    filestream.seekg(0, std::ios_base::end);
    const size_t len = filestream.tellg();
    filestream.seekg(magic_length);

    Vec<u8> vbuffer;
    vbuffer.resize(len - magic_length);

    filestream.read(reinterpret_cast<char*>(vbuffer.data()), len - magic_length);

    return this->decode_png(vbuffer);
}


Image PNG_Decoder::decode_png(Vec<u8>& file_buffer) {
    // D(std::println("Decoding PNG");)
    Logger::log(LOG_LEVEL::INFO, "Decoding PNG of size {} bytes", file_buffer.size());
    auto start = std::chrono::high_resolution_clock::now();

    size_t pxl_idx {0};
    ChunkPNG chunk {};

    Vec<u8> rawidat;
    rawidat.reserve(file_buffer.size());

    do {
        chunk = this->read_chunk(file_buffer, pxl_idx);

        Logger::log(LOG_LEVEL::INFO, "Got chunk {:#x} of length {} bytes", static_cast<u32>(chunk.type), chunk.length);

        switch (chunk.type) {

            case ChunkType::IHDR:
                this->decode_ihdr(chunk.data);
                break;

            case ChunkType::PLTE:
                break;
            case ChunkType::IDAT: {
                rawidat.append_range(chunk.data);
                break;
            }
            case ChunkType::IEND:
                break;

        }
    } while (chunk.type != ChunkType::IEND);

    libdeflate_decompressor *decompressor = libdeflate_alloc_decompressor();

    libdeflate_result result = libdeflate_zlib_decompress(
        decompressor,
        rawidat.data(),
        rawidat.size(),
        inflated_data.data(),
        inflated_data.size(),
        nullptr
    );

    if (result != LIBDEFLATE_SUCCESS) {
        std::println(std::cerr, "Deflate died");
        exit(result);
    }

    libdeflate_free_decompressor(decompressor);

    // Reverse the filters
    Image img (width, height);
    
    D(
        int n=0;
        int s=0;
        int u=0;
        int a=0;
        int p=0;
    )

    
    const size_t line_in_size = width * bpp + 1;     // Width of the image + 1 byte for the filter type
    const size_t line_out_size = width * img.nb_channels;

    u16 line_id = 0;

    std::span<const u8> data_view(inflated_data);
    auto res = get_scanline(data_view, line_in_size);

    const u8 in_out_chan_diff = img.nb_channels - bpp;

    while (res.has_value()) {
        std::span<const u8> scanline = res.value();

        PNG_FILT_TYPE filt = static_cast<PNG_FILT_TYPE>(scanline[0]);
        scanline = scanline.subspan(1, scanline.size()-1);

        u8 *start = img.get_raw_handle() + (line_id * line_out_size);
        u8 *up_start = start - line_out_size;
        u8 *end = start + line_out_size;

        std::span<u8> output_line(start, end);
        std::span<const u8> up_line(up_start, up_start + line_out_size);    // May contain garbage data for line_id = 0

        switch (filt) {

        case PNG_FILT_TYPE::NONE: {
            D(n++;)

            switch (color_type) {
                case PNG_COLOR_TYPE::RGBA: {
                    std::memcpy(output_line.data(), scanline.data(), scanline.size());
                    break;
                }

                case PNG_COLOR_TYPE::RGB: {

                    for (size_t i = 0; i < scanline.size(); i += bpp) {
                        const auto ipxl = (i / bpp) * (bpp + in_out_chan_diff);
                        std::memcpy(output_line.data() + ipxl, scanline.data() + i, bpp);
                    }
                    break;
                }

                default:
                    break;
            }
            break;
        }

        case PNG_FILT_TYPE::SUB: {
            D(s++;)

            switch (color_type) {

                case PNG_COLOR_TYPE::RGBA:
                case PNG_COLOR_TYPE::RGB: {
                    
                    for (size_t i = 0; i < scanline.size(); i++) {
                        const auto dest = i + (i / bpp) * in_out_chan_diff;
                        const u8 left = (i < bpp) ? 0 : output_line[dest - img.nb_channels];
                        output_line[dest] = left + scanline[i];
                    }

                    break;
                }

                default:
                    break;
            }

            break;
        }

        case PNG_FILT_TYPE::UP: {
            D(u++;)

            switch (color_type) {

                case PNG_COLOR_TYPE::RGBA: {

                    if (line_id == 0) {
                        std::memcpy(output_line.data(), scanline.data(), scanline.size());
                    }
                    else {
                        for (size_t i = 0; i < output_line.size(); i++) {
                            output_line[i] = up_line[i] + scanline[i];
                        }
                    }
                    break;
                }

                default:
                    break;

            }

            break;
        }

        case PNG_FILT_TYPE::AVG: {
            D(a++;)

            switch (color_type) {

                case PNG_COLOR_TYPE::RGBA: {

                    for (size_t i = 0; i < output_line.size(); i++) {
                        const uint16_t left = (i < bpp) ? 0 : output_line[i - img.nb_channels];
                        const uint16_t up = (line_id == 0) ? 0 : up_line[i];
                        output_line[i] = scanline[i] + ((left + up) >> 1);
                    }

                    break;
                }

                default:
                    break;
            }
            break;
        }


        case PNG_FILT_TYPE::PAETH: {
            D(p++;)

            switch (color_type) {

                case PNG_COLOR_TYPE::RGBA: {

                    for (size_t i = 0; i < output_line.size(); i++) {
                        const bool first_pixel = i < bpp;
                        const bool first_scanline = line_id == 0;
        
                        const u8 left = first_pixel ? 0 : output_line[i - img.nb_channels];
                        const u8 up = first_scanline ? 0 : up_line[i];
                        const u8 upleft = (first_pixel || first_scanline) ? 0 : up_line[i - img.nb_channels];
        
                        output_line[i] = this->paeth_predictor(left, up, upleft) + scanline[i];
                    }

                    break;
                }

                default:
                    break;
            }
            break;
        }
        }

        line_id++;
        res = get_scanline(data_view, line_in_size);
    }

    D(Logger::log(LOG_LEVEL::INFO, "Filter count - {} NONE - {} SUB - {} UP - {} AVG - {} PAETH", n,s,u,a,p);)
    auto end = std::chrono::high_resolution_clock::now();
    std::println("Decoded PNG of size {}x{} in {}", width, height, std::chrono::duration_cast<std::chrono::milliseconds>(end - start));
    return img;
}



std::optional<std::span<const u8>> PNG_Decoder::get_scanline(std::span<const u8>& data, size_t scanline_size) {
    if (data.size() == 0) 
        return std::nullopt;

    std::span<const u8> scanline = data.subspan(0, scanline_size);
    data = data.subspan(scanline_size, data.size() - scanline_size);

    return scanline;
}




ChunkPNG PNG_Decoder::read_chunk(Vec<u8>& data, size_t& idx) {
    ChunkPNG chunk {};
    chunk.length = read<u32, std::endian::big>(data, idx);
    chunk.type = static_cast<ChunkType>(read<u32, std::endian::big>(data, idx));
    chunk.data = std::span<u8>(data.begin() + idx, chunk.length);
    idx += chunk.length;
    chunk.crc = read<u32, std::endian::big>(data, idx);

    return chunk;
}



void PNG_Decoder::decode_ihdr(std::span<u8> data) {
    size_t idx {0};
    width = read<u32, std::endian::big>(data, idx);
    height = read<u32, std::endian::big>(data, idx);
    bit_depth = read<u8, std::endian::big>(data, idx);
    color_type = static_cast<PNG_COLOR_TYPE>(read<u8>(data, idx));
    compression_method = read<u8>(data, idx);
    filter_method = read<u8>(data, idx);
    interlace_method = read<u8>(data, idx);
    bpp = channel_nb.at(color_type) * bit_depth / 8;
    inflated_data.resize(height * width * bpp + height);
}



u8 PNG_Decoder::paeth_predictor(u8 a, u8 b, u8 c) {
    const i16 p = a + b - c;
    const i16 pa = std::abs(p - a);
    const i16 pb = std::abs(p - b);
    const i16 pc = std::abs(p - c);

    if (pa <= pb && pa <= pc) return a;
    else if (pb <= pc) return b;
    else return c;
}


}