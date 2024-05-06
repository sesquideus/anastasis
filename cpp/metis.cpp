#include "utils/resample.h"
#include "types/types.h"

using namespace Astar;

std::vector<DetectorImage> prepare(
        const DetectorImage & original,
        const pair<int> size
) {
    std::vector<DetectorImage> result;
    result.reserve(6);

    Point centre = Point(original.size()) / 2;

    for (int i = -1; i <= 1; ++i) {
        Point shift = {static_cast<real>(i) * original.width() / size.first, 0};
        ModelImage downsampled(size);

        downsampled.naive_drizzle(original - shift);
        result.emplace_back(centre + shift, original.size(), 0, pair<int>(1, 1), downsampled.data());
    }

    for (int i = -1; i <= 1; ++i) {
        Point shift = {0, static_cast<real>(i)};
        ModelImage downsampled(size);

        // These are rotated by one quarter
        downsampled.naive_drizzle((original - shift) >> TauFourth);
        result.emplace_back(centre + shift, original.size(), TauFourth, pair<int>(1, 1), downsampled.data());
    }


    return result;
}

void print_usage(int code) {
    fmt::print("Usage: metis <filename> model_size_x model_size_y pixfrac_x pixfrac_y\n");
    fmt::print("<filename>          path to an 8-bit bmp file\n");
    fmt::print("model_size_x        int > 0, number of pixels in horizontal direction for downsampled images\n");
    fmt::print("model_size_y        int > 0, number of pixels in vertical direction for downsampled images\n");
    fmt::print("pixfrac_x           float (0, 1], pixel fraction used in drizzling, horizontal direction "
               "(recommended 1 / subpixel_shifts_x)\n");
    fmt::print("pixfrac_y           float (0, 1], pixel fraction used in drizzling, vertical direction "
               "(recommended 1 / subpixel_shifts_y)\n");
    std::exit(code);
}

int main(int argc, char * argv[]) {
    std::string exec_name = argv[0];
    std::vector<std::string> args(argv, argv + argc);

    pair<int> model_size;
    pair<real> pixfrac;
    try {
        if (argc != 6) {
            print_usage(0);
        }
        model_size = {std::stoi(args[2]), std::stoi(args[3])};
        pixfrac = {std::stof(args[4]), std::stof(args[5])};
    } catch (std::invalid_argument & exc) {
        fmt::print("Aborting due to invalid argument type: {}\n", exc.what());
        print_usage(1);
    }

    Point centre = Point(model_size) / 2;

    try {
        DetectorImage original(centre, model_size, 0, {1, 1}, args[1]);
        std::vector<DetectorImage> downsampled = prepare(original, model_size);

        int index = 0;
        for (auto && it: downsampled) {
            it.print_world();
            it.save_npy(fmt::format("out/downsample-{}.npy", index++));
        }

    } catch (std::runtime_error & exc) {
        fmt::print("Aborting: {}\n", exc.what());
        std::exit(3);
    }

    return 0;
}