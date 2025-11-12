#pragma once

#include <cstdint>

namespace ivmg::imgproc::filt {

class Conv {
    public:
        uint16_t ksize;
        uint16_t radius;
        double* kernel;

        Conv(uint16_t ks): ksize(ks % 2 != 0 ? ks : ks + 1),
            radius(static_cast<uint16_t>(ksize / 2)),
            kernel(new double[ksize * ksize])
        {
        }

        ~Conv() {
            delete[] kernel;
        }
};

}
