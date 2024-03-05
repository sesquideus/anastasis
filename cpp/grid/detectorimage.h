#ifndef ANASTASIS_CPP_DETECTORIMAGE_H
#define ANASTASIS_CPP_DETECTORIMAGE_H

#include <random>
#include "grid/grid.h"
#include "image.h"


/**
 * Extends an abstract grid with actual pixel data. Implemented in separate structures to enable vectorization
 */
class DetectorImage: public PlacedGrid, public Image {
private:
    Matrix _data;
    static pair<int> read_bitmap_header(const std::string & filename);
public:
    DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                  pair<std::size_t> grid_size);
    DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                  const Matrix & data);
    DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                  const std::string & filename);

    DetectorImage & apply(const std::function<real(void)> & function);
    DetectorImage & apply(const std::function<real(real)> & function);
    DetectorImage & apply(const std::function<real(int, int, real)> & function);

    DetectorImage & multiply(real value);

    void fill(real value = 0.0);

    void randomize();
};

DetectorImage operator*(const DetectorImage & image, real scale);
DetectorImage operator*(const DetectorImage & image, pair<real> scale);
DetectorImage operator+(const DetectorImage & image, Point shift);

template<>
class fmt::formatter<DetectorImage> {
public:
    constexpr auto parse(fmt::format_parse_context & ctx) -> fmt::format_parse_context::iterator {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}') return it;
        return it;
    }
    auto format(const DetectorImage & image, format_context & ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(), "Detector image at {:f} with size {:d}×{:d}, rotation {:.3f}, pixfrac ({:.3f}, {:.3f})",
                              image.centre(), image.size().first, image.size().second,
                              image.rotation(), image.pixfrac().first, image.pixfrac().second);
    }
};
#endif //ANASTASIS_CPP_DETECTORIMAGE_H
