#pragma once

#include <expected>
#include <filesystem>
#include <fstream>
#include <functional>
#include <ivmg/core/image.hpp>
#include <ivmg/core/formats.hpp>
#include <memory>
#include <unordered_map>

#include "png.hpp"
#include "qoi.hpp"
#include "pam.hpp"
#include "utils.hpp"

using namespace ivmg::types;

namespace ivmg {

class Decoder;



class DecoderRegistry {
private:

public:
    static ResultOr<Image, IVMG_DEC_ERR> decode(std::ifstream& file) {
        static Vec<Unique<Decoder>> decs;

        if (decs.empty()) {
            decs.emplace_back(std::make_unique<PNG_Decoder>());
        }

        for (const auto& d : decs) {
            if (d->can_decode(file))
                return d->decode(file);
        }

        return std::unexpected(IVMG_DEC_ERR::UNKNOWN_FORMAT);
    }

};


// Encoder functions registration
typedef std::function<void(const Image&, const std::filesystem::path&)> Encoder_fn;
const std::unordered_map<Formats, Encoder_fn> encoders = {
    { Formats::QOI, encode_qoi },
    { Formats::PAM, encode_pam }
};

}
