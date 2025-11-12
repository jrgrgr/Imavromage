#include <ivmg/core/image.hpp>

#include "pam.hpp"

#include <cstring>
#include <sstream>
#include <print>


std::vector<uint8_t> ivmg::PamEncoder::encode(const Image& img) {

	std::println("Encoding in PAM");

	std::stringstream ss;
	ss << "P7\n"
        << "WIDTH " << img.width() << "\n"
        << "HEIGHT " << img.height() << "\n"
        << "DEPTH " << 4 << "\n"
        << "MAXVAL 255\n"
        << "TUPLTYPE RGB_ALPHA\n"
        << "ENDHDR\n";

    std::string hdr = ss.str();

    std::vector<uint8_t> out(hdr.length() + img.size_bytes());
    std::memcpy(out.data(), hdr.data(), hdr.length());
    std::memcpy(out.data() + hdr.length(), img.get_raw_handle(), img.size_bytes());

    return out;
}
