#include "placedimage.h"

namespace Astar {
    PlacedImage::PlacedImage(const AffineTransform & transform, pair<int> size, pair<real> pixfrac):
        AbstractGrid(size),
        PlacedGrid(transform, pixfrac),
        Image(size)
    { }

    PlacedImage::PlacedImage(const AffineTransform & transform, const Matrix & data, pair<real> pixfrac):
        AbstractGrid(data.cols(), data.rows()),
        PlacedGrid(transform, pixfrac),
        Image(data)
    { }

} // Astar