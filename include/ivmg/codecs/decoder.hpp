#pragma once

#include <fstream>

namespace ivmg {

class Image;

/**
 * @brief Interface class defining what a Decoder needs
 */
class Decoder {
public:
    virtual ~Decoder() = default;

    /**
     * @brief Reads the first bytes of the file to tell whether it can decode it.
     *
     * @param filestream the binary stream of the file to probe
     * @return true if it can decode it, false otherwise
     */
    virtual bool can_decode(std::ifstream& filestream) const = 0;

    /**
     * @brief Decode the raw bytes of the given image file
     *
     * @param filestream the binary stream of the file to decode
     * @return Image the decoded image
     */
    virtual Image decode(std::ifstream& filestream) = 0;
};





}
