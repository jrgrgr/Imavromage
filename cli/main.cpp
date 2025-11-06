#include "argparse.hpp"
#include <iostream>
#include <ivmg/filters/gaussian_blur.hpp>
#include <ivmg/ivmg.hpp>
#include <ivmg/core/image.hpp>
#include <ivmg/core/formats.hpp>
#include <getopt.h>
#include <print>



int main(int argc, char** argv) {

    argparse::ArgumentParser program("ivmg-cli");
    program.add_argument("-i", "--input")
        .help("specify the input file")
        .required();

    program.add_argument("-o", "--output")
        .help("specify the output file (or - for stdout)")
        .default_value("out.pam");

    program.add_argument("-k", "--ksize")
        .help("width of the kernel")
        .scan<'d', int>()
        .default_value(7);

    program.add_argument("-s", "--sigma")
        .help("variance of the kernel")
        .scan<'d', int>()
        .default_value(100);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    std::string input_file = program.get<std::string>("--input");

    std::string output_file = program.get<std::string>("--output");
    if(output_file == "-")
        output_file = "/dev/stdout";

    // int k = program.get<int>("--ksize");
    // int si = program.get<int>("--sigma");

    ivmg::Image img = ivmg::open(input_file);

    if (!img.save("test_out.pam").has_value())
    	std::println(std::cerr, "Error when saving the file");


    // auto s = std::chrono::high_resolution_clock::now();
    // ivmg::Image img2 = img | GaussianBlur(k, si);
    // auto e = std::chrono::high_resolution_clock::now();
    // std::println("Applied Gaussian blur with k={} and s={} in {}", k, si, std::chrono::duration_cast<std::chrono::milliseconds>(e-s));
    // ivmg::save(img2, output_file, ivmg::Formats::PAM);


    return 0;

}
