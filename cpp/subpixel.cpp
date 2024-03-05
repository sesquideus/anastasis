#include "grid/detectorimage.h"
#include "grid/modelimage.h"


ModelImage one_to_one(const DetectorImage & image) {
    auto copy = image;
    ModelImage original(image.size().first, image.size().second);
    copy.set_centre({static_cast<real>(image.size().first) / 2.0, static_cast<real>(image.size().second) / 2.0});
    copy.set_physical_size(image.size());
    fmt::print("{}\n", copy);
    fmt::print("{}\n", original);
    original += copy;
    original.save_bmp("one-to-one.bmp");
    return original;
}

int main(int argc, char * argv[]) {
    std::string exec_name = argv[0];
    std::vector<std::string> args(argv + 1, argv + argc);

    if (argc != 8) {
        fmt::print("Usage: subpixel filename [model size x] [model size y] [subpixel shifts x] [subpixel shifts y] "
                   "[pixfrac x] [pixfrac y]\n");
        std::terminate();
    }

    int model_width = std::stoi(args[1]);
    int model_height = std::stoi(args[2]);
    int subpixels_x = std::stoi(args[3]);
    int subpixels_y = std::stoi(args[4]);
    real pixfrac_x = std::stof(args[5]);
    real pixfrac_y = std::stof(args[6]);

    Point centre = {static_cast<real>(model_width) / 2, static_cast<real>(model_height) / 2};
    DetectorImage input(centre, {model_width, model_height}, 0, {1, 1}, args[0]);
    input.set_centre({input.physical_size().first / 2, input.physical_size().second / 2});

    auto size = input.size();
    real scale_x = static_cast<real>(size.first) / model_width;
    real scale_y = static_cast<real>(size.second) / model_height;
    fmt::print("About to drizzle {} (image size {} × {}) down to model with size {} × {}, placed at {}\n",
               args[0], size.first, size.second, model_width, model_height, centre);
    fmt::print("Pixel scaling factors are x = {}, y = {}\n", scale_x, scale_y);
    fmt::print("Pixfrac is px = {}, py = {}\n", pixfrac_x, pixfrac_y);

    auto original = one_to_one(input);

    input.set_centre(centre);
    input.set_physical_size({model_width, model_height});
    std::vector<std::vector<ModelImage>> downsampled;

    for (int i = 0; i < subpixels_x; ++i) {
        downsampled.emplace_back();
        for (int j = 0; j < subpixels_y; ++j) {
            downsampled[i].emplace_back(model_width, model_height);
            real shift_x = static_cast<real>(i) / static_cast<real>(subpixels_x);
            real shift_y = static_cast<real>(j) / static_cast<real>(subpixels_y);
            downsampled[i][j] += (input + Point(shift_x, shift_y));
            fmt::print("Scaled down to {:d} × {:d} with shift {:6.3f} {:6.3f}, pixfrac ({:6.3f}, {:6.3f})\n",
                       model_width, model_height, shift_x, shift_y, input.pixfrac().first, input.pixfrac().second);
            // downsized[i][j] /= static_cast<real>(scale_x * scale_y);
            downsampled[i][j].save_raw("out/downsampled.raw");
        }
    }

    ModelImage drizzled(size.first, size.second);
    for (int i = 0; i < subpixels_x; ++i) {
        for (int j = 0; j < subpixels_y; ++j) {
            real shift_x = static_cast<real>(i) / static_cast<real>(subpixels_x);
            real shift_y = static_cast<real>(j) / static_cast<real>(subpixels_y);
            auto new_centre = Point((centre.x - shift_x) * scale_x, (centre.y - shift_y) * scale_y);
            DetectorImage image(
                    new_centre,
                    size, 0, {pixfrac_x, pixfrac_y}, downsampled[i][j].data()
            );
            drizzled += image.multiply(1.0 / (subpixels_x * subpixels_y * pixfrac_x * pixfrac_y));
            fmt::print("{}\n", image);
            fmt::print("Drizzled with shifts {:6.3f} {:6.3f} (pixels {:6.3f} {:6.3f}) at {:6.3f} {:6.3f}, pixfrac {:6.3f} {:6.3f}\n",
                       shift_x, shift_y,
                       shift_x * scale_x, shift_y * scale_y,
                       new_centre.x, new_centre.y,
                       pixfrac_x, pixfrac_y);
        }
    }
    drizzled.save_bmp("out/drizzled.bmp");
    drizzled.save_npy("out/drizzled.npy");
    drizzled.save_raw("out/drizzled.raw");

    real similarity = original ^ drizzled;
    fmt::print("Similarity score is {}\n", similarity);

    real angle = (original * drizzled) / (std::sqrt(drizzled * drizzled) * std::sqrt(original * original));
    fmt::print("Angle difference is {}\n", std::acos(angle));
}