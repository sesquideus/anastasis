#ifndef ANASTASIS_CPP_PIXEL_H
#define ANASTASIS_CPP_PIXEL_H

#include <array>
#include "types/point.h"
#include "grid/box.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>

namespace Astar {
    class Box;

    class Pixel {
    private:
        std::array<std::array<Point, 2>, 2> corners_;
        constexpr static real Slack = 1e-12;
    public:
        explicit Pixel();
        explicit Pixel(Point bottomleft, Point bottomright, Point topleft, Point topright);
        explicit Pixel(const Box & box);
        inline static Pixel invalid() { return Pixel(Point::invalid(), Point::invalid(), Point::invalid(), Point::invalid()); };

        [[nodiscard]] inline Point a() const { return this->corners_[0][0]; }
        [[nodiscard]] inline Point b() const { return this->corners_[0][1]; }
        [[nodiscard]] inline Point c() const { return this->corners_[1][1]; }
        [[nodiscard]] inline Point d() const { return this->corners_[1][0]; }

        [[nodiscard]] inline std::array<std::array<Point, 2>, 2> corners() const { return this->corners_; };
        [[nodiscard]] Box bounding_box(real slack = Pixel::Slack) const;
        [[nodiscard]] bool contains(Point point) const;

        [[nodiscard]] real area() const;

        /** Operator shorthand for overlap **/
        [[nodiscard]] real operator&(const Pixel & other) const;
        /** Operator shorthand for area of union **/
        [[nodiscard]] real operator|(const Pixel & other) const;
        [[nodiscard]] real overlap(const Pixel & other) const; // Area of overlap
        [[nodiscard]] real orthogonal_overlap(const Pixel & other) const; // Area of overlap

    };
}

real linear_overlap(pair<real> int1, pair<real> int2);

template<>
class fmt::formatter<Astar::Pixel> {
private:
    char presentation = 'f';
public:
    constexpr auto parse(fmt::format_parse_context & ctx) -> fmt::format_parse_context::iterator {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;
        if (it != end && *it != '}') return it;
        return it;
    }

    auto format(const Astar::Pixel & pixel, format_context & ctx) const -> format_context::iterator {
        return presentation == 'f'
               ? fmt::format_to(ctx.out(), "(({:f}, {:f}), ({:f}, {:f}))",
                                pixel.a(), pixel.b(), pixel.d(), pixel.c())
               : fmt::format_to(ctx.out(), "(({:e}, {:e}), ({:e}, {:e}))",
                                pixel.a(), pixel.b(), pixel.d(), pixel.c());
    }
};

#endif //ANASTASIS_CPP_PIXEL_H
