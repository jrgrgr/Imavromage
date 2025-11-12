#pragma once

#include <ivmg/codecs/errors.hpp>
#include <ivmg/imgproc/filter.hpp>

#include <vector>
#include <filesystem>
#include <expected>
#include <unordered_map>

namespace ivmg {

using namespace imgproc::filt;

enum class ColorType : uint8_t {
    RGBA = 0,
    RGB  = 1,
    YUV  = 2
};

const std::unordered_map<ColorType, uint8_t> colortype_to_chan_nb {
    { ColorType::RGBA, 4 },
    { ColorType::RGB,  3 },
    { ColorType::YUV,  3 }
};


//======================================================
// MAIN IMAGE CLASS
//======================================================

/**
In memory buffer of raw decoded image data
*/
class Image {

    private:
        std::vector<uint8_t> data;  // In row major. x is col, y is row
        uint32_t w;     // In pixels
        uint32_t h;    // In pixels


    public:
        ColorType color_type;
        uint8_t nb_channels;

        Image(const uint32_t w, const uint32_t h, ColorType ct = ColorType::RGBA):
            data(w * h * colortype_to_chan_nb.at(ct), 255), w(w), h(h), color_type(ct)
        {
            nb_channels = colortype_to_chan_nb.at(color_type);
        }

        // ACCESSORS
        constexpr uint8_t* get_raw_handle() { return data.data(); }
        constexpr const uint8_t* get_raw_handle() const { return data.data(); }
        constexpr uint32_t width() const { return w; }
        constexpr uint32_t height() const { return h; }
        constexpr size_t size_bytes() const { return data.size(); }
        constexpr size_t size_pixels() const { return data.size() / colortype_to_chan_nb.at(color_type); }      // Assuming RGBA for now


        std::expected<void, IVMG_ENC_ERR> save(const std::filesystem::path& imgpath);


        Image operator|(const Conv& f);


};


}
