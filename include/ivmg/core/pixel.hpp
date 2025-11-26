#pragma once

#include <span>
#include <cstdint>

/**
 * @brief Simple non owning proxy class for pixels on an image
 */
class Pixel {
private:
	std::span<uint8_t> data;

public:

	// TODO: Support other color types
	explicit Pixel(uint8_t* start): data(start, 4) {}

	inline uint8_t r() const { return data[0]; }
	inline uint8_t g() const { return data[1]; }
	inline uint8_t b() const { return data[2]; }
	inline uint8_t a() const { return data[3]; }


	bool operator==(const Pixel& other) {
		return r() == other.r() && g() == other.g() && b() == other.b() && a() == other.a();
	}

};
