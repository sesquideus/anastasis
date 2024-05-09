#include "affine.h"

namespace Astar {
    AffineTransformation::AffineTransformation(Point translation, pair<real> scale, real rotation):
            translation_(translation),
            linear_(rotation_matrix(rotation) * scaling_matrix(scale))
    { }

    AffineTransformation::AffineTransformation(Point translation, const Matrix2D & linear):
            translation_(translation),
            linear_(linear)
    { }
}