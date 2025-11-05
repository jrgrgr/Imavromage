#pragma once

#include <cstdint>
#include <filesystem>
#include <ivmg/codecs/encoder.hpp>

namespace ivmg {

class Image;

void encode_pam(const Image& img, const std::filesystem::path& outfile);

class PAM_Encoder: public Encoder {

public:
	inline PAM_Encoder() {};

	std::vector<uint8_t> encode(const Image& img) override;
};



}
