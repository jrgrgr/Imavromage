#include <ivmg/ivmg.hpp>

#include <fstream>
#include "logger.hpp"
#include "codecs/codecs.hpp"
#include "utils.hpp"

using namespace ivmg;
using namespace ivmg::types;

enum class Formats;

LOG_LEVEL Logger::level = LOG_LEVEL::NONE;

Image ivmg::open(const std::string& imgpath) {
    std::ifstream file(imgpath, std::ios::binary);

    std::vector<u8> file_buffer;

    auto res = CodecRegistry::decode(file);
    if (res.has_value()) {
        return res.value();
    }
    else {
        switch (res.error()) {
            case IVMG_DEC_ERR::UNKNOWN_FORMAT:
            break;
        }
    }


    Logger::log(LOG_LEVEL::ERROR, "Unknown format");
    exit(1);
};



std::expected<void, IVMG_ENC_ERR> ivmg::save(const Image &img, const std::filesystem::path &imgpath) {

    Formats target = ext_to_format.at(imgpath.extension());

    if (encoders.contains(target)) {
        encoders.at(target)(img, imgpath);
        return {};
    }
    else
        return std::unexpected(IVMG_ENC_ERR::UNSUPPORTED_FORMAT);
}
