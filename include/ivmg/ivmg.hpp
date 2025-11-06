#pragma once

#include "core/image.hpp"

namespace ivmg {

class Image;
enum class Formats;
enum class IVMG_ENC_ERR;

Image open(const std::string& imgpath);
std::expected<void, IVMG_ENC_ERR> save(const Image &img, const std::filesystem::path &imgpath);


}
