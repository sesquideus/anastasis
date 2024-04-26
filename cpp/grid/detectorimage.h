#ifndef ANASTASIS_CPP_DETECTORIMAGE_H
#define ANASTASIS_CPP_DETECTORIMAGE_H

#include "grid/placedgrid.h"
#include "image.h"


/**
 * Extends an abstract placed grid with actual pixel data, hence a combination of an Image and a PlacedGrid.
 * Implemented in separate structures in hope to be cache-friendly.
 */
class DetectorImage: public PlacedGrid<DetectorImage>, public Image<DetectorImage> {
public:
    DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                  pair<int> grid_size);
    DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                  const Matrix & data);
    DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                  const std::string & filename);
};

template<>
class fmt::formatter<DetectorImage> {
public:
    constexpr auto parse(fmt::format_parse_context & ctx) -> fmt::format_parse_context::iterator {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}') return it;
        return it;
    }
    auto format(const DetectorImage & image, format_context & ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(),
                              "Detector image at {:f} with size {:d}×{:d}, rotation {:.3f}, pixfrac ({:.3f}, {:.3f})",
                              image.centre(), image.size().first, image.size().second,
                              image.rotation(), image.pixfrac().first, image.pixfrac().second);
    }
};
#endif //ANASTASIS_CPP_DETECTORIMAGE_H
