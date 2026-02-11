#ifndef ANASTASIS_CPP_POLYPIXEL_H
#define ANASTASIS_CPP_POLYPIXEL_H
#include <vector>

#include "types/point.h"

namespace Astar {
    /* An arbitrary convex polygonal pixel (not just a parallelogram).
     * Slower, but more versatile for arbitrary projections or crazy instruments.
     * And technically this is not the bottleneck anyway, so why not.
     */
    class PolyPixel {
        const std::vector<Point> _vertices;
        const int _degree;

    public:
        explicit PolyPixel(const std::vector<Point> & points):
            _vertices(points),
            _degree(points.size()) {}

        [[nodiscard]] int degree(void) const { return this->_degree; }
        const Point & operator[](int index) const;
        [[nodiscard]] Point centre(void) const;

        [[nodiscard]] real area(void) const;

        // Return a copy but with origin shifted to zero
        [[nodiscard]] PolyPixel to_zero(void) const;

        // Contains a point?
        [[nodiscard]] bool contains(const Point & point) const;

        friend PolyPixel operator & (const PolyPixel & first, const PolyPixel & second);
    };

    // Overlap of two pixels (polygon)
}

#endif //ANASTASIS_CPP_POLYPIXEL_H