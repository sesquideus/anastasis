#include "utils/resample.h"
#include "types/types.h"

using namespace Astar;

std::vector<DetectorImage> prepare(
        const DetectorImage & original,
        const pair<int> resolution,
        const pair<real> pixfrac = {1, 1}
) {
    std::vector<DetectorImage> result;
    result.reserve(6);
    int new_max = std::max(resolution.first, resolution.second);
    pair<int> new_size = {new_max, new_max};
    Point new_centre = Point(new_size) / 2;

    for (int i = -1; i <= 1; ++i) {
        auto copy = original;
        copy.set_physical_size(resolution);
        Point shift = {static_cast<real>(i) / 3, 0};
        ModelImage downsampled(resolution);

        fmt::print("Drizzle: {}\n", copy - shift);
        downsampled.naive_drizzle(copy - shift);
        result.emplace_back(new_centre + shift * 3,
                            new_size, 0, pixfrac, downsampled.data());
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
        result.emplace_back(new_centre + shift.rotated(rotation) * 3,
                            new_size, rotation, pixfrac, downsampled.data());
    }

    return result;
}

ModelImage prepare_direct(
        const DetectorImage & original,
        const pair<int> resolution
) {
    ModelImage out(resolution);
    auto copy = original;
    copy.set_physical_size(resolution);
    copy.set_centre(Point(resolution) / 2);

    out.naive_drizzle(copy);
    return out;
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
        ModelImage raw_original(original);
        std::vector<DetectorImage> downsampled = prepare(original, model_size, pixfrac);

        int index = 0;
        for (auto && it: downsampled) {
            it.print_world();
            it.save_npy(fmt::format("out/downsample-{}.npy", index++));
        }

        pair<int> output_size = {84, 84};
        ModelImage output(output_size);
        output.naive_drizzle(downsampled) /= (6.0 * original.count() / output.count());
        output.save_npy("out/drizzled.npy");

        raw_original.save_npy("out/original.npy");

        ModelImage direct = prepare_direct(original, output_size);
        direct /= (static_cast<real>(original.count()) / output.count());
        direct.save_npy("out/direct.npy");

        auto difference = direct - output;
        difference.save_npy("out/difference.npy");

        fmt::print("Maximum absolute error is {}\n", difference.maximum());
        fmt::print("RMS error is {}\n", difference.rms());

    } catch (std::runtime_error & exc) {
        fmt::print("Aborting: {}\n", exc.what());
        std::exit(3);
    }

    return 0;
}