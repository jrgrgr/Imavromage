#pragma once

#include <ivmg/codecs/encoder.hpp>
#include <filesystem>

namespace ivmg {

class Image;

void encode_pam(const Image& img, const std::filesystem::path& outfile);

class PamEncoder: public Encoder {

public:
	inline PamEncoder() {};

	std::vector<uint8_t> encode(const Image& img) override;
};



}
