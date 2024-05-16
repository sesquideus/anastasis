#ifndef ANASTASIS_CPP_RESAMPLE_H
#define ANASTASIS_CPP_RESAMPLE_H

#include "grid/placedimage.h"

namespace Astar {
    /** Convert a DetectorImage to a ModelImage with a naive drizzle **/
    PlacedImage one_to_one(const PlacedImage & image);

    /** Take a detector image and create <subsamples_x> * <subsamples_y> downsampled images with resolution
     *  <model_width> * <model_height>, each shifted by a corresponding fraction of a *new* pixel size in both directions.
     *  This can be then used to reconstruct the original image.
     */
    std::vector<PlacedImage> downsample_grid(
        const PlacedImage & image,
        pair<int> model_size,
        pair<int> subsamples,
        pair<real> pixfrac
    );

    /** Take an original image and downsample_grid it to many small subexposures at different rotations and positions
     *  (currently of the same size)
     *  For instance for METIS we want three subsamples in horizontal direction and three subsamples rotated 90°.
     */
    std::vector<PlacedImage> downsample_with_rotations(
        const PlacedImage & original,
        pair<int> model_size,
        const std::vector<std::tuple<real, real, real>> & positions,
        pair<real> pixfrac
    );
}

#endif //ANASTASIS_CPP_RESAMPLE_H
