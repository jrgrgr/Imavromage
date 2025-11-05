#include "codecs.hpp"
#include "ivmg/core/formats.hpp"
#include "png.hpp"
#include "utils.hpp"
#include <expected>
#include <memory>

namespace ivmg {


	CodecRegistry::CodecRegistry() {
		decoders.emplace_back(std::make_unique<PNG_Decoder>());
		// encoders.emplace_back(std::make_unique<QOI_Encoder>());
	}


	CodecRegistry& CodecRegistry::get_instance() {
		static CodecRegistry instance;
		return instance;
	}


	ResultOr<Image, IVMG_DEC_ERR> CodecRegistry::decode(std::ifstream &file) {
		CodecRegistry& registry = get_instance();

		for (const auto& dec: registry.decoders) {
			if (dec->can_decode(file))
				return dec->decode(file);
		}

		return std::unexpected(IVMG_DEC_ERR::UNKNOWN_FORMAT);
	}





}
