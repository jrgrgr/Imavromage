#pragma once

#include <vector>
#include <cstdint>

namespace ivmg {

class Image;

/**
 * @brief Interface class defining what an Encoder is
 */
class Encoder {
public:
    virtual ~Encoder() = default;

    /**
     * @brief Encode the given image
     *
     * @param img the image to encode
     * @return vector of bytes encoded accordingly
     */
    virtual std::vector<uint8_t> encode(const Image& img) = 0;
};






}
