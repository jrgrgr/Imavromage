#pragma once

#include <cstddef>
#include <iterator>
#include <ivmg/codecs/errors.hpp>
#include <ivmg/imgproc/filter.hpp>
#include <ivmg/core/pixel.hpp>

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
* @brief In memory buffer of raw decoded image data
*/
class Image {

    private:
        std::vector<uint8_t> data;  // In row major. x is col, y is row
        uint32_t w;     // In pixels
        uint32_t h;    // In pixels
        ColorType color_type;
        uint8_t nb_channels;

    public:
        Image(const uint32_t w, const uint32_t h, ColorType ct = ColorType::RGBA);

        Image operator=(Image& img);

        // ACCESSORS
        inline constexpr uint8_t* get_raw_handle() { return data.data(); }
        inline constexpr const uint8_t* get_raw_handle() const { return data.data(); }
        inline constexpr uint32_t width() const { return w; }
        inline constexpr uint32_t height() const { return h; }
        inline constexpr uint8_t nb_chan() const { return nb_channels; }
        inline constexpr size_t size_bytes() const { return data.size(); }
        inline constexpr size_t size_pixels() const { return data.size() / colortype_to_chan_nb.at(color_type); }      // Assuming RGBA for now

        /**
         * @brief Save the image at the given path.
         *
         * Extrapolates the wanted format from the extension
         *
         * @param imgpath the path where to save the image
         * @return std::expected object, empty if ok, an error code otherwise
         */
        std::expected<void, IVMG_ENC_ERR> save(const std::filesystem::path& imgpath);


        Image operator|(const Conv& f);


        /**
         * @brief Iterator for image
         *
         * TODO: Support other color types than just RGBA
         */
        class iterator {
        private:
        	uint8_t* ptr;

        public:
        	explicit iterator(uint8_t* p): ptr(p) {}

        	using iterator_category = std::forward_iterator_tag;
         	using value_type = Pixel;
          	using difference_type = std::ptrdiff_t;
           	using pointer = Pixel*;
            using reference = Pixel;

            reference operator*() { return Pixel(ptr); }

            iterator& operator++() {
            	ptr += 4;
             	return *this;
            }

            iterator& operator++(int i) {
            	iterator& tmp = *this;
             	ptr += 4;
              	return tmp;
            }

            bool operator==(const iterator& other) { return ptr == other.ptr; }
            bool operator!=(const iterator& other) { return ptr != other.ptr; }

            iterator operator+(difference_type n) const { return iterator(ptr + n * 4); }
            iterator& operator+=(difference_type n) {
            	ptr += n * 4;
             	return *this;
            }

            iterator operator-(difference_type n) const { return iterator(ptr - n * 4); }
            iterator& operator-=(difference_type n) {
            	ptr -= n * 4;
             	return *this;
            }

            difference_type operator-(const iterator& other) {
            	return (ptr - other.ptr) / 4;
            }
        };

        inline iterator begin() { return iterator(data.data()); }
        inline iterator end() { return iterator(data.data() + size_pixels() * nb_channels); }


};


}
