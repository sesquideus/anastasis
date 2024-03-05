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
    original.save_bmp("out/one-to-one.bmp");
    original.save_npy("out/one-to-one.npy");
    return original;
}

std::vector<std::vector<DetectorImage>> downsample(
    const DetectorImage & image,
    int model_width, int model_height,
    int subsamples_x, int subsamples_y
) {
    std::vector<std::vector<DetectorImage>> downsampled;
    Point centre = {static_cast<real>(model_width) / 2, static_cast<real>(model_height) / 2};

    for (int j = 0; j < subsamples_y; ++j) {
        downsampled.emplace_back();
        for (int i = 0; i < subsamples_x; ++i) {
            ModelImage temp(model_width, model_height);
            real shift_x = static_cast<real>(i) / static_cast<real>(subsamples_x);
            real shift_y = static_cast<real>(j) / static_cast<real>(subsamples_y);
            Point shift = {shift_x, shift_y};

            temp += (image + Point(shift_x, shift_y));
            fmt::print("Scaled down to {:d} × {:d} with shift {:6.3f} {:6.3f}, pixfrac ({:6.3f}, {:6.3f})\n",
                       model_width, model_height, shift_x, shift_y, image.pixfrac().first, image.pixfrac().second);
            downsampled[i].emplace_back(centre + shift, pair<int>(model_width, model_height), 0, pair<int>(1, 1), temp.data());
        }
    }
    return downsampled;
}

ModelImage drizzle(
        const std::vector<std::vector<DetectorImage>> & downsampled,
        pair<int> output_size
) {
    ModelImage drizzled(output_size.first, output_size.second);
    int samples_x = downsampled.size();
    int samples_y = downsampled[0].size();

    for (unsigned int j = 0; j < downsampled.size(); ++j) {
        for (unsigned int i = 0; i < downsampled[j].size(); ++i) {
            real shift_x = static_cast<real>(i) / static_cast<real>(samples_x);
            real shift_y = static_cast<real>(j) / static_cast<real>(samples_y);
            real scale_x = static_cast<real>(output_size.first) / downsampled[i][j].width();
            real scale_y = static_cast<real>(output_size.second) / downsampled[i][j].height();
            drizzled += downsampled[i][j];
            fmt::print("Drizzled with shifts {:6.3f} {:6.3f} (pixels {:6.3f} {:6.3f}) "
                       "at {:6.3f} {:6.3f}), pixfrac {:6.3f} {:6.3f}\n",
                       shift_x, shift_y,
                       shift_x * scale_x, shift_y * scale_y,
                       downsampled[i][j].centre().x, downsampled[i][j].centre().y,
                       downsampled[i][j].pixfrac().first, downsampled[i][j].pixfrac().second);
        }
    }
    return drizzled;
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

    auto downsampled = downsample(input, model_width, model_height, subpixels_x, subpixels_y);
    downsampled[0][0].save_npy("out/downsampled.npy");

    auto drizzled = drizzle(downsampled, {model_width, model_height});
    drizzled.save_npy("out/drizzled.npy");

    real similarity = original ^ drizzled;
    fmt::print("Similarity score is {}\n", similarity);

    real angle = (original * drizzled) / (std::sqrt(drizzled * drizzled) * std::sqrt(original * original));
    fmt::print("Angle difference is {}\n", std::acos(angle));
}