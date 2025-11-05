#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <ivmg/core/image.hpp>
#include <ivmg/core/formats.hpp>
#include <unordered_map>

#include "qoi.hpp"
#include "pam.hpp"
#include "utils.hpp"

using namespace ivmg::types;

namespace ivmg {

class Encoder;
class Decoder;

/**
 * @brief Simple registry class holding the available encoders and decoders
 * in a Meyer's singleton pattern.
 *
 * Contains a std::vector of Decoder objects
 * Contains a std::vector of Encoder objects
 */
class CodecRegistry {
private:

	std::vector<std::unique_ptr<Decoder>> decoders;
	std::vector<std::unique_ptr<Encoder>> encoders;

	/**
	 * @brief Function enabling the Meyer's singleton pattern.
	 * Private to enable a cleaner syntax
	 */
	static CodecRegistry& get_instance();
	CodecRegistry();


	CodecRegistry(const CodecRegistry&) = delete;
	CodecRegistry& operator=(const CodecRegistry&) = delete;


public:

	/**
	 * @brief Choose the appropriate decoder and use it to decode the given image.
	 *
	 * @param file the binary stream of the image file to decode
	 * @return std::expected with the decoded image as the expected value, an error code otherwise
	 */
	static ResultOr<Image, IVMG_DEC_ERR> decode(std::ifstream& file);


	/**
	 * @brief Encode the given image using the appropriate encoder as chosen
	 * according to the given path's extension
	 *
	 * @param img the image to encode
	 * @param imgpath the file to encode the image to
	 * @return std::expected with void as the expected value, an error code otherwise
	 */
	static ResultOr<void, IVMG_ENC_ERR> encode(const Image& img, const std::filesystem::path& imgpath);


	/**
	 * @brief Registers a decoder by appending it to the end of the known list
	 *
	 * @tparam T the decoder to register. Must inherit from ivmg::Decoder
	 */
	template <class T> requires std::is_base_of_v<Decoder, T>
	static void register_decoder() {
		CodecRegistry& registry = get_instance();
		registry.decoders.emplace_back(std::make_unique<T>());
	}


	/**
	 * @brief Registers an encoder by appending it to the end of the known list
	 *
	 * @tparam T the encoder to register. Must inherit from ivmg::Encoder
	 */
	template <class T> requires std::is_base_of_v<Encoder, T>
	static void register_encoder() {
		CodecRegistry& registry = get_instance();
		registry.encoders.emplace_back(std::make_unique<T>());
	}
};



// class DecoderRegistry {
// private:
// 	std::vector<std::unique_ptr<Decoder>> decoders;

//         static DecoderRegistry &get_instance();

//       public:
// 	static ResultOr<Image, IVMG_DEC_ERR> decode(std::ifstream &file) {
// 		static std::vector<std::unique_ptr<Decoder>> decs;

// 		if (decs.empty()) {
// 			decs.emplace_back(std::make_unique<PNG_Decoder>());
// 		}

// 		for (const auto &d : decs) {
// 			if (d->can_decode(file))
// 				return d->decode(file);
// 		}

// 		return std::unexpected(IVMG_DEC_ERR::UNKNOWN_FORMAT);
// 	}
// };


// Encoder functions registration
typedef std::function<void(const Image&, const std::filesystem::path&)> Encoder_fn;
const std::unordered_map<Formats, Encoder_fn> encoders = {
	{ Formats::QOI, encode_qoi },
	{ Formats::PAM, encode_pam }
};

}
