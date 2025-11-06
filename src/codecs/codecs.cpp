#include "codecs.hpp"
#include "ivmg/core/formats.hpp"
#include "pam.hpp"
#include "png.hpp"
#include <expected>
#include <fstream>
#include <memory>

namespace ivmg {


	CodecRegistry::CodecRegistry() {
		decoders.emplace_back(std::make_unique<PNG_Decoder>());

		encoders.emplace(".pam", std::make_unique<PAM_Encoder>());
	}


	CodecRegistry& CodecRegistry::get_instance() {
		static CodecRegistry instance;
		return instance;
	}


	std::expected<Image, IVMG_DEC_ERR> CodecRegistry::decode(std::ifstream &file) {
		CodecRegistry& registry = get_instance();

		for (const auto& dec: registry.decoders) {
			if (dec->can_decode(file))
				return dec->decode(file);
		}

		return std::unexpected(IVMG_DEC_ERR::UNKNOWN_FORMAT);
	}


	std::expected<void, IVMG_ENC_ERR> CodecRegistry::encode(const Image& img, const std::filesystem::path& imgpath) {
		CodecRegistry& registry = get_instance();

		std::string ext = imgpath.extension();

		if (registry.encoders.contains(ext)) {
			std::vector<uint8_t> encoded = registry.encoders.at(ext)->encode(img);
			std::ofstream outfile(imgpath);
			outfile.write(reinterpret_cast<char*>(encoded.data()), encoded.size());
			return {};
		}

		return std::unexpected(IVMG_ENC_ERR::UNSUPPORTED_FORMAT);
	}


}
