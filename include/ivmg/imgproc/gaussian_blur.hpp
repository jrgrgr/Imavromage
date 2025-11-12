#pragma once

#include "filter.hpp"
#include <algorithm>
#include <cstddef>
#include "math.h"

namespace ivmg::imgproc::filt {


class GaussianBlur: public Conv {

private:
    double s;

public:
    GaussianBlur(uint8_t ks, double sigma = 0.0): Conv(ks), s(sigma) {

        if (s == 0.0) s = std::max(ksize / 2.0, 1.0);

        double sum = 0.0;

        // Compute the kernel values
        for (size_t i = 0; i < ksize * ksize; i++) {
            const int16_t x = i / ksize - radius;
            const int16_t y = i % ksize - radius;

            const double d = 2 * M_PI * (s * s);
            const double epwr = -((x*x) + (y*y)) / (2*(s*s));

            const double kval = std::exp(epwr) / d;;
            kernel[i] = kval;
            sum += kval;
        }

        // Normalise the kernel
        for (size_t i = 0; i < ksize * ksize; i++) {
            kernel[i] /= sum;
        }
    }


};





}
