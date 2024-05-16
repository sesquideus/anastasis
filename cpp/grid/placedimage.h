//
// Created by kvik on 15/05/24.
//

#ifndef ANASTASIS_CPP_PLACEDIMAGE_H
#define ANASTASIS_CPP_PLACEDIMAGE_H

#include "grid/placedgrid.h"
#include "grid/image.h"

namespace Astar {
    /** Represents a placed image: has a dimension, position and flux data.
     *  Position and physical dimension can be altered, as well as values of the data,
     *  but the logical dimension cannot.
     */
    class PlacedImage: public PlacedGrid<PlacedImage>, public Image<PlacedImage> {

    public:
        // Create from canonical transform with zero data
        PlacedImage(const AffineTransform & transform, pair<int> size, pair<real> pixfrac = {1.0, 1.0});
        // Create from canonical transform and data
        PlacedImage(const AffineTransform & transform, const Matrix & data, pair<real> pixfrac = {1.0, 1.0});
        // Create from canonical transform and bitmap file
        PlacedImage(const AffineTransform & transform, const std::string & filename, pair<real> pixfrac = {1.0, 1.0});

        [[nodiscard]] Pixel pixel(int x, int y) const;

        PlacedImage & naive_drizzle(const PlacedImage & image);
        PlacedImage & naive_drizzle(const std::vector<PlacedImage> & images);
        // PlacedImage & weighted_drizzle(const PlacedImage & image);
        // PlacedImage & weighted_drizzle(const std::vector<PlacedImage> & image);

    };

    // Difference is possible but only when transforms and pixfracs are the same
    PlacedImage operator-(const PlacedImage & lhs, const PlacedImage & rhs) {
        assert(lhs.transform() == rhs.transform());
        assert(lhs.pixfrac() == rhs.pixfrac());

        return PlacedImage(lhs.transform(), lhs.data() - rhs.data());
    }
}

#endif //ANASTASIS_CPP_PLACEDIMAGE_H
