#ifndef ANASTASIS_CPP_BOX_H
#define ANASTASIS_CPP_BOX_H

#include "types/types.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>

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

        friend struct fmt::formatter<Box>;
    };
}

template<>
struct fmt::formatter<Astar::Box> {
    auto format(const Astar::Box & box, format_context & ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(), "(x {:d} to {:d}, y {:d} to {:d})",
                              box.left, box.right, box.bottom, box.top);
    }
};

#endif //ANASTASIS_CPP_BOX_H
