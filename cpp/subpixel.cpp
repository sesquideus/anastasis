#include "grid/detectorimage.h"
#include "grid/modelimage.h"


const int model_width = 64;
const int model_height = 64;


int main(int argc, char * argv[]) {
    std::string exec_name = argv[0];
    std::vector<std::string> args(argv + 1, argv + argc);

    if (argc < 2) {
        fmt::print("Usage: subpixel filename [xscale] [yscale]\n");
        std::terminate();
    }
    int subpixels_x = 4;
    int subpixels_y = 4;
    real pixfrac_x = 1.0;
    real pixfrac_y = 1.0;

    if (argc >= 4) {
        subpixels_x = std::stoi(args[1]);
        subpixels_y = std::stoi(args[2]);
    }
    if (argc == 6) {
        pixfrac_x = std::stof(args[3]);
        pixfrac_y = std::stof(args[4]);
    }

    Point centre = {static_cast<real>(model_width) / 2, static_cast<real>(model_height) / 2};
    DetectorImage input(centre, {model_width, model_height}, 0, {1, 1}, args[0]);
    auto size = input.size();
    real scale_x = static_cast<real>(size.first) / model_width;
    real scale_y = static_cast<real>(size.second) / model_height;
    pair<real> scale = {scale_x, scale_y};
    fmt::print("About to drizzle {} (image size {} × {}) at ({}, {}) down to model {} × {}\n",
               args[0], size.first, size.second, centre.x, centre.y, model_width, model_height);
    fmt::print("Pixel scaling factors are x = {}, y = {}\n", scale_x, scale_y);

    ModelImage original(size.first, size.second);
    original += input * pair<real>(scale_x, scale_y);
    //original.save_bmp("out.bmp");

    std::vector<std::vector<ModelImage>> downs;
    for (int i = 0; i < subpixels_x; ++i) {
        downs.emplace_back();
        for (int j = 0; j < subpixels_y; ++j) {
            downs[i].emplace_back(model_width, model_height);
            real shift_x = static_cast<real>(i) / static_cast<real>(subpixels_x);
            real shift_y = static_cast<real>(j) / static_cast<real>(subpixels_y);
            downs[i][j] += (input + Point(shift_x, shift_y));
            fmt::print("Scaled down to {:d} × {:d} with shift {:6.3f} {:6.3f}\n", model_width, model_height, shift_x, shift_y);
        }
    }

    ModelImage drizzled(size.first, size.second);
    for (int i = 0; i < subpixels_x; ++i) {
        for (int j = 0; j < subpixels_y; ++j) {
            real shift_x = static_cast<real>(i) / static_cast<real>(subpixels_x);
            real shift_y = static_cast<real>(j) / static_cast<real>(subpixels_y);
            auto new_centre = Point(centre.x * scale_x, centre.y * scale_y) - Point(scale_x * shift_x, scale_y * shift_y);
            DetectorImage image(
                    new_centre,
                    size, 0, {pixfrac_x, pixfrac_y}, downs[i][j].data()
            );
            drizzled += image;
            fmt::print("Drizzled with shifts {:6.3f} {:6.3f} (pixels {:6.3f} {:6.3f}) at {:6.3f} {:6.3f}\n",
                       shift_x, shift_y, shift_x * scale_x, shift_y * scale_y, new_centre.x, new_centre.y);
        }
    }
    drizzled.save_raw("drizzle.raw");

    real similarity = original ^ drizzled;
    fmt::print("Similarity score is {}\n", similarity);
}