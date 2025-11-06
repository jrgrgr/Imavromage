#pragma once

#include <string>
#include <unordered_map>

namespace ivmg {

enum class Formats {
    PNG,
    QOI,
    PAM
};


enum class IVMG_DEC_ERR {
    UNKNOWN_FORMAT
};


enum class IVMG_ENC_ERR {
    UNSUPPORTED_FORMAT
};


const std::unordered_map<std::string, Formats> ext_to_format {
    { ".png", Formats::PNG },
    { ".qoi", Formats::QOI },
    { ".pam", Formats::PAM }
};

}
