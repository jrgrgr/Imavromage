#include <ivmg/codecs/codecs.hpp>
#include <ivmg/core/image.hpp>

#include "pam/pam.hpp"
#include "png/png.hpp"
#include "qoi/qoi.hpp"


namespace ivmg {


	CodecRegistry::CodecRegistry() {
		decoders.emplace_back([]() { return std::make_unique<PngDecoder>(); });

		encoders.emplace(".pam", []() { return std::make_unique<PamEncoder>(); });
		encoders.emplace(".qoi", []() { return std::make_unique<QoiEncoder>(); });
	}


	CodecRegistry& CodecRegistry::get_instance() {
		static CodecRegistry instance;
		return instance;
	}


	std::expected<Image, IVMG_DEC_ERR> CodecRegistry::decode(std::ifstream &file) {
		CodecRegistry& registry = get_instance();

		for (const auto& factory: registry.decoders) {
			std::unique_ptr<Decoder> dec = factory();
			if (dec->can_decode(file))
				return dec->decode(file);
		}

		return std::unexpected(IVMG_DEC_ERR::UNKNOWN_FORMAT);
	}


	std::expected<void, IVMG_ENC_ERR> CodecRegistry::encode(const Image& img, const std::filesystem::path& imgpath) {
		CodecRegistry& registry = get_instance();

		std::string ext = imgpath.extension();

		if (registry.encoders.contains(ext)) {
			std::unique_ptr<Encoder> enc = registry.encoders.at(ext)();
			std::vector<uint8_t> encoded = enc->encode(img);
			std::ofstream outfile(imgpath);
			outfile.write(reinterpret_cast<char*>(encoded.data()), encoded.size());
			return {};
		}

		return std::unexpected(IVMG_ENC_ERR::UNSUPPORTED_FORMAT);
	}


}
