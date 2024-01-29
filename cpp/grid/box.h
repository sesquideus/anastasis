#ifndef ANASTASIS_CPP_BOX_H
#define ANASTASIS_CPP_BOX_H

#include "grid/pixel/pixel.h"

/** The Box class represents a rectangular box aligned with the coordinate grid.
 *  Similar to Pixel, but optimized for this specific case
 */


class Pixel;


class Box {
private:
    int left_;
    int right_;
    int bottom_;
    int top_;
public:
    explicit Box(int left, int right, int bottom, int top):
        left_(left), right_(right), bottom_(bottom), top_(top) {}
    Pixel as_pixel() const;
    real overlap(const Box & other) const;

    [[nodiscard]] inline int left() const { return this->left_; }
    [[nodiscard]] inline int right() const { return this->right_; }
    [[nodiscard]] inline int bottom() const { return this->bottom_; }
    [[nodiscard]] inline int top() const { return this->top_; }
};


template<>
class fmt::formatter<Box> {
private:
    char presentation = 'f';
public:
    constexpr auto parse(fmt::format_parse_context & ctx) -> fmt::format_parse_context::iterator {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;
        if (it != end && *it != '}') return it;
        return it;
    }

    auto format(const Box & box, format_context & ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(), "(x {:d} to {:d}, y {:d} to {:d})",
                                box.left(), box.right(), box.bottom(), box.top());
    }
};

#endif //ANASTASIS_CPP_BOX_H
