#ifndef ANASTASIS_CPP_BOX_H
#define ANASTASIS_CPP_BOX_H

#include "grid/pixel/pixel.h"

/** The Box class represents a rectangular box aligned with the coordinate grid.
 *  Similar to Pixel, but optimized for this specific case
 */

namespace Astar {
    class Pixel;

    class Box {
    public:
        int left;
        int right;
        int bottom;
        int top;

        explicit Box(int left, int right, int bottom, int top):
                left(left), right(right), bottom(bottom), top(top) {}
        [[nodiscard]] Pixel as_pixel() const;
        [[nodiscard]] real overlap(const Box & other) const;
    };
}

template<>
class fmt::formatter<Astar::Box> {
public:
    auto format(const Astar::Box & box, format_context & ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(), "(x {:d} to {:d}, y {:d} to {:d})",
                              box.left, box.right, box.bottom, box.top);
    }
};

#endif //ANASTASIS_CPP_BOX_H
