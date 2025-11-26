#include <cstdint>
#include <cstring>
#include <ivmg/core/image.hpp>
#include <ivmg/imgproc/filter.hpp>
#include <ivmg/codecs/codecs.hpp>

#include <print>
#include <thread>

namespace ivmg {
using namespace imgproc::filt;

Image Image::operator|(const Conv& f) {

    Image out(w, h);
    const size_t num_threads = std::thread::hardware_concurrency();
    const size_t pixels_per_thread = (w * h) / num_threads;


    auto convolve_scalar_worker = [] (const Image& img, const Conv& filter, Image& out, size_t start_pxl, size_t end_pxl) {

        std::vector<float> pxl_tmp(img.nb_channels);

        std::println("Processing pixels {} to {}", start_pxl, end_pxl);

        for (size_t i = start_pxl*img.nb_channels; i < end_pxl*img.nb_channels; i += img.nb_channels) {
            int ix = (i % (img.w * img.nb_channels)) / img.nb_channels;
            int iy = (i / (img.w * img.nb_channels));

            std::ranges::fill(pxl_tmp, 0.0f);

            for (int k = 0; k < filter.ksize * filter.ksize; k++) {
                // auto [kx, ky] = GetCoordsInFlatArray(k, filter.ksize);
                int kx = k % filter.ksize - filter.radius;
                int ky = k / filter.ksize - filter.radius;
                // kx -= filter.radius;
                // ky -= filter.radius;

                const uint32_t kix = ix + kx;
                const uint32_t kiy = iy + ky;

                // Boundary check. Acts as 0 padding.
                if ( (kix < 0) || (kiy < 0) || (kix >= img.w) || (kiy >= img.h) ) continue;

                for (size_t c = 0; c < img.nb_channels; c++) {
                    auto iidx = (kiy * img.w + kix) * img.nb_channels + c;

                    pxl_tmp[c] += img.data[iidx] * filter.kernel[k];
                }
            }

            for (size_t c = 0; c < img.nb_channels; c++) {
                out.data[i + c] = static_cast<uint8_t>(std::clamp(pxl_tmp[c], 0.0f, 255.0f));
            }
        }
    };


    {
        std::vector<std::jthread> threads(num_threads);

        for (size_t i = 0; i < num_threads; i++) {
            size_t start = i * pixels_per_thread;
            size_t end = (i == num_threads - 1) ? w * h : start + pixels_per_thread;

            threads.emplace_back(convolve_scalar_worker, std::cref(*this), std::cref(f), std::ref(out), start, end);
        }
    }

    return out;

}

std::expected<void, IVMG_ENC_ERR> Image::save(const std::filesystem::path& imgpath) {
	return CodecRegistry::encode(*this, imgpath);
}



Image::Image(const uint32_t width, const uint32_t height, ColorType ct): w(width), h(height), color_type(ct)
{
    nb_channels = colortype_to_chan_nb.at(color_type);
    data.resize(width * height * nb_channels, 255);
};


Image Image::operator=(Image& other) {

	Image out(other.w, other.h, other.color_type);
	size_t idx = 0;

	for (Pixel p : other) {
		data[idx++] = p.r();
		data[idx++] = p.g();
		data[idx++] = p.b();
		data[idx++] = p.a();
	}

	return out;
}


}
