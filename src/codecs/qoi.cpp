#include "qoi.hpp"
#include "core/common.hpp"
#include <cstdint>
#include <ivmg/core/image.hpp>

#include <print>
#include <array>
#include <cstddef>
#include <cstring>


namespace ivmg {

uint16_t QOI_Encoder::hash_pixel(const qoi_color_t& c) {
	return (c.r * 3 + c.g * 5 + c.b * 7 + c.a * 11) & 63;
}

qoi_diff_t QOI_Encoder::color_diff(const qoi_color_t &c1, const qoi_color_t &c2) {
    int8_t r = c1.r - c2.r;
    int8_t g = c1.g - c2.g;
    int8_t b = c1.b - c2.b;

    return { r, g, b };
}


std::vector<uint8_t> QOI_Encoder::encode(const Image& img) {
	std::println("Encoding in QOI");

	std::vector<uint8_t> out(img.size_bytes() + QOI_Encoder::hdr_size + 8);

	auto write32 = [&] (uint32_t val) {
		out.at(ptr++) = (0xff000000 & val) >> 24;
		out.at(ptr++) = (0x00ff0000 & val) >> 16;
		out.at(ptr++) = (0x0000ff00 & val) >> 8;
		out.at(ptr++) = (0x000000ff & val);
	};

	// Header
	write32(magic);
	write32(img.width());
	write32(img.height());
	out.at(ptr++) = channels;
	out.at(ptr++) = static_cast<uint8_t>(colorspace);


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
				out[ptr++] = QOI_OP_RUN | (run - 1);
				run = 0;
			}
		}
		else {
			if (run > 0) {
				out[ptr++] = QOI_OP_RUN | (run - 1);
				run = 0;
			}

			uint16_t hash = hash_pixel(cur_pxl);

			if (color_cache.at(hash) == cur_pxl) {
				out[ptr++] = QOI_OP_INDEX | hash;
			}
			else {
				color_cache[hash] = cur_pxl;

				if (cur_pxl.a == prev_pxl.a) {
					const qoi_diff_t diff = color_diff(cur_pxl, prev_pxl);
                    const int8_t dr_dg = diff.r - diff.g;
                    const int8_t db_dg = diff.b - diff.g;

                    if (
                        diff.r >= -2 && diff.r <= 1 &&
                        diff.g >= -2 && diff.g <= 1 &&
                        diff.b >= -2 && diff.b <= 1
                    ) {
                        out[ptr++] = QOI_OP_DIFF | (diff.r + 2) << 4 | (diff.g + 2) << 2 | (diff.b + 2);
                    }

                    else if (
                        diff.g > -33 && diff.g < 32 &&
                        dr_dg > -9 && dr_dg < 8 &&
                        db_dg > -9 && db_dg < 8
                    ) {
                        out[ptr++] = QOI_OP_LUMA | (diff.g + 32);
                        out[ptr++] = (dr_dg + 8) << 4 | (db_dg + 8);
                    }

                    else {
                        out[ptr++] = (QOI_OP_RGB);
                        out[ptr++] = (cur_pxl.r);
                        out[ptr++] = (cur_pxl.g);
                        out[ptr++] = (cur_pxl.b);
                    }
				}
				else {
					out[ptr++] = (QOI_OP_RGBA);
                    out[ptr++] = (cur_pxl.r);
                    out[ptr++] = (cur_pxl.g);
                    out[ptr++] = (cur_pxl.b);
                    out[ptr++] = (cur_pxl.a);
				}
			}
		}
		prev_pxl = cur_pxl;
	}
	std::memcpy(out.data() + ptr, end_marker.data(), end_marker.size());
	ptr += end_marker.size();
	return out;
}



}
