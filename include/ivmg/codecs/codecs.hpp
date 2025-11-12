#pragma once


#include <ivmg/codecs/errors.hpp>

#include <fstream>
#include <unordered_map>
#include <memory>
#include <expected>
#include <vector>
#include <filesystem>
#include <functional>

namespace ivmg {

class Image;
class Encoder;
class Decoder;

/**
 * @brief Simple registry class holding the available encoders and decoders
 * in a Meyer's singleton pattern.
 *
 * Contains a std::vector of Decoder objects
 * Contains a map of string to Encoder objects
 */
class CodecRegistry {
private:

	using DecoderFactory = std::function<std::unique_ptr<Decoder>()>;
	using EncoderFactory = std::function<std::unique_ptr<Encoder>()>;

	std::vector<DecoderFactory> decoders;
	std::unordered_map<std::string, EncoderFactory> encoders;

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
	static std::expected<Image, IVMG_DEC_ERR> decode(std::ifstream& file);


	/**
	 * @brief Encode the given image using the appropriate encoder as chosen
	 * according to the given path's extension
	 *
	 * @param img the image to encode
	 * @param imgpath the file to encode the image to
	 * @return std::expected with void as the expected value, an error code otherwise
	 */
	static std::expected<void, IVMG_ENC_ERR> encode(const Image& img, const std::filesystem::path& imgpath);


	/**
	 * @brief Registers a decoder by appending it to the end of the known list
	 *
	 * @tparam T the decoder to register. Must inherit from ivmg::Decoder
	 */
	template <class T> requires std::is_base_of_v<Decoder, T>
	static void register_decoder() {
		CodecRegistry& registry = get_instance();
		registry.decoders.emplace_back([]() { return std::make_unique<T>(); });
	}


	/**
	 * @brief Registers an encoder by appending it to the end of the known list
	 *
	 * @tparam T the encoder to register. Must inherit from ivmg::Encoder
	 */
	template <class T> requires std::is_base_of_v<Encoder, T>
	static void register_encoder(const std::string& ext) {
		CodecRegistry& registry = get_instance();
		registry.encoders.insert_or_assign(ext, []() { return std::make_unique<T>(); });
	}
};

}
