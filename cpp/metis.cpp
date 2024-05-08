#include "utils/resample.h"
#include "types/types.h"

using namespace Astar;

std::vector<DetectorImage> prepare(
        const DetectorImage & original,
        const pair<int> resolution
) {
    std::vector<DetectorImage> result;
    result.reserve(6);
    pair<int> new_size = {std::max(original.width(), original.height()), std::max(original.width(), original.height())};
    Point centre = Point(original.size()) / 2;
    Point new_centre = Point(new_size) / 2;

    for (int i = -1; i <= 1; ++i) {
        auto copy = original;
        copy.set_physical_size(resolution);
        Point shift = {static_cast<real>(i) / 3, 0};
        ModelImage downsampled(resolution);

        fmt::print("Drizzle: {}\n", copy - shift);
        (copy - shift).print_corners();
        downsampled.naive_drizzle(copy - shift);
        result.emplace_back(centre + shift * 600     / 28, original.size(), 0, pair<int>(1, 1), downsampled.data());
    }

    real rotation = TauFourth;
    for (int i = -1; i <= 1; ++i) {
        auto copy = original;
        copy.set_physical_size(resolution.second, resolution.first);
        Point shift = {static_cast<real>(i) / 3, 0};
        ModelImage downsampled(resolution);

        // These are rotated by one quarter
        fmt::print("Drizzle: {}\n", (copy - shift) << rotation);
        downsampled.naive_drizzle((copy - shift) << rotation);
        ((copy - shift) << rotation).print_corners();
        result.emplace_back(new_centre + shift.rotated(rotation) * 600 / 28, original.size(), rotation, pair<int>(1, 1), downsampled.data());
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

        ModelImage output(std::max(original.width(), original.height()), std::max(original.width(), original.height()));
        output.naive_drizzle(downsampled);

        output.save_npy("out/drizzled.npy");

    } catch (std::runtime_error & exc) {
        fmt::print("Aborting: {}\n", exc.what());
        std::exit(3);
    }

    return 0;
}