#include "qoi.hpp"
#include "core/common.hpp"
#include <array>
#include <bit>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>

using namespace ivmg;


void ivmg::encode_qoi(const Image &img, const std::filesystem::path &outfile) {
    std::println("Encoding in QOI");
    std::ofstream out(outfile, std::ios::out | std::ios::binary);

    qoi_header hdr {
        .width = std::byteswap(img.width()),
        .height = std::byteswap(img.height()),
        .channels = 4,
        .colorspace = QOI_COLORSPACE::SRGB
    };

    qoi_color_t prev_pxl = { 0, 0, 0, 255 };
    size_t run = 0;

    // out.write(reinterpret_cast<char*>(&hdr), sizeof(hdr));

    std::array<qoi_color_t, 64> color_cache {};

    std::vector<u8> test(img.width() * img.height() * img.nb_channels + sizeof(hdr) + 8);

    size_t p = 0;
    std::memcpy(test.data(), &hdr, sizeof(hdr));
    p += sizeof(hdr);

    for (size_t i = 0; i < img.size_bytes(); i += BYTE_PER_PIXEL) {
        qoi_color_t cur_pxl = {
            img.get_raw_handle()[i],
            img.get_raw_handle()[i + 1],
            img.get_raw_handle()[i + 2],
            img.get_raw_handle()[i + 3]
        };

        if (cur_pxl == prev_pxl) {
            run++;
            if (run == 62 || i == img.size_bytes() - BYTE_PER_PIXEL) {
                // out.put(QOI_OP_RUN | (run - 1));
                test[p++] = QOI_OP_RUN | (run - 1);
                run = 0;
            }
        }
        else {
            if (run > 0) {
                // out.put(QOI_OP_RUN | (run - 1));
                test[p++] = QOI_OP_RUN | (run - 1);
                run = 0;
            }

            int pxl_hash = QOI_PIXEL_HASH(cur_pxl);

            if (color_cache.at(pxl_hash) == cur_pxl)
                // out.put(QOI_OP_INDEX | pxl_hash);
                test[p++] = QOI_OP_INDEX | pxl_hash;


            else {
                color_cache.at(pxl_hash) = cur_pxl;

                if (cur_pxl.a == prev_pxl.a) {
                    const qoi_diff_t diff = QOI_COLOR_DIFF(cur_pxl, prev_pxl);
                    const i8 dr_dg = diff.r - diff.g;
                    const i8 db_dg = diff.b - diff.g;

                    if (
                        diff.r >= -2 && diff.r <= 1 &&
                        diff.g >= -2 && diff.g <= 1 &&
                        diff.b >= -2 && diff.b <= 1
                    ) {
                        // out.put(QOI_OP_DIFF | (diff.r + 2) << 4 | (diff.g + 2) << 2 | (diff.b + 2));
                        test[p++] = QOI_OP_DIFF | (diff.r + 2) << 4 | (diff.g + 2) << 2 | (diff.b + 2);
                    }

                    else if (
                        diff.g > -33 && diff.g < 32 &&
                        dr_dg > -9 && dr_dg < 8 &&
                        db_dg > -9 && db_dg < 8
                    ) {
                        // out.put(QOI_OP_LUMA | (diff.g + 32));
                        test[p++] = QOI_OP_LUMA | (diff.g + 32);
                        // out.put((((dr_dg + 8) << 4) | (db_dg + 8)) );
                        test[p++] = (dr_dg + 8) << 4 | (db_dg + 8);
                    }

                    else {
                        // out.put(QOI_OP_RGB);
                        // out.put(cur_pxl.r);
                        // out.put(cur_pxl.g);
                        // out.put(cur_pxl.b);
                        test[p++] = (QOI_OP_RGB);
                        test[p++] = (cur_pxl.r);
                        test[p++] = (cur_pxl.g);
                        test[p++] = (cur_pxl.b);
                    }
                }
                else {
                    // out.put(QOI_OP_RGBA);
                    // out.put(cur_pxl.r);
                    // out.put(cur_pxl.g);
                    // out.put(cur_pxl.b);
                    // out.put(cur_pxl.a);
                    test[p++] = (QOI_OP_RGBA);
                    test[p++] = (cur_pxl.r);
                    test[p++] = (cur_pxl.g);
                    test[p++] = (cur_pxl.b);
                    test[p++] = (cur_pxl.a);
                }
            }
        }

        prev_pxl = cur_pxl;
    }

    const char QOI_END_MARKER[8] = {0,0,0,0,0,0,0,1};
    // out.write(QOI_END_MARKER, 8);
    std::memcpy(test.data() + p, QOI_END_MARKER, 8);
    p += 8;

    out.write(reinterpret_cast<char*>(test.data()), test.size());
}

constexpr qoi_diff_t ivmg::QOI_COLOR_DIFF(const qoi_color_t &c1, const qoi_color_t &c2) {
    i8 r = c1.r - c2.r;
    i8 g = c1.g - c2.g;
    i8 b = c1.b - c2.b;

    return { r, g, b };
}



constexpr size_t ivmg::QOI_PIXEL_HASH(const qoi_color_t &c) {
    return (c.r * 3 + c.g * 5 + c.b * 7 + c.a * 11) % 64;
}
