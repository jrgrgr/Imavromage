#pragma once

#include <ivmg/core/image.hpp>
#include <string>


namespace ivmg {


Image open(const std::string& imgpath);
std::expected<void, IVMG_ENC_ERR> save(const Image &img, const std::filesystem::path &imgpath);


}
