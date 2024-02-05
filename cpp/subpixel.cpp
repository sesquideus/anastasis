#include <array>
#include "grid/detectorimage.h"
#include "grid/modelimage.h"


int main(int argc, char * argv[]) {
    std::string exec_name = argv[0];
    std::vector<std::string> args(argv + 1, argv + argc);

    DetectorImage input({128, 128}, {256, 256}, 0, {1, 1}, "metis.bmp");

    std::vector<std::vector<ModelImage>> downs;
    for (int i = 0; i <= 2; ++i) {
        downs.emplace_back();
        for (int j = 0; j <= 2; ++j) {
            downs[i].emplace_back(256, 256);
            real shift_x = static_cast<real>(i) / 3.0;
            real shift_y = static_cast<real>(j) / 3.0;
            downs[i][j] += (input + Point(shift_x, shift_y));
            downs[i][j].save(fmt::format("out-{}-{}.raw", i, j));
            fmt::print("Scaled down to 256×256 with shift {} {}\n", shift_x, shift_y);
        }
    }

    ModelImage drizzled(2048, 1024);
    for (int i = 0; i <= 2; ++i) {
        for (int j = 0; j <= 2; ++j) {
            real shift_x = static_cast<real>(i) / 3.0;
            real shift_y = static_cast<real>(j) / 3.0;
            DetectorImage image({1024 + 4 * shift_x, 512 + 4 * shift_y}, {2048, 1024}, 0, {0.5, 0.5}, downs[i][j].data());
            drizzled += image;
            fmt::print("Now drizzled {} {}\n", shift_x, shift_y);
        }
    }
    drizzled.save("metis.raw");
}