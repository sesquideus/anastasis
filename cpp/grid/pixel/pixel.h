#ifndef ANASTASIS_CPP_PIXEL_H
#define ANASTASIS_CPP_PIXEL_H

#include <array>

#include "types/point.h"
#include "grid/box.h"
#include "utils/eigen.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>

namespace Astar {
    class Box;

    class Pixel {
    private:
        Quadrangle corners_;
        constexpr static real Slack = 1e-12;
    public:
        explicit Pixel();
        explicit Pixel(const Vector & bottom_left, const Vector & bottom_right,
                       const Vector & top_left, const Vector & top_right);
        explicit Pixel(Quadrangle quadrangle);
        explicit Pixel(const Box & box);
        inline static Pixel invalid() {
            return Pixel(
                {Astar::Invalid, Astar::Invalid},
                {Astar::Invalid, Astar::Invalid},
                {Astar::Invalid, Astar::Invalid},
                {Astar::Invalid, Astar::Invalid});
        }

        [[nodiscard]] inline Quadrangle corners() const { return this->corners_; }

        [[nodiscard]] inline Vector a() const { return this->corners().col(0); }
        [[nodiscard]] inline Vector b() const { return this->corners().col(1); }
        [[nodiscard]] inline Vector c() const { return this->corners().col(2); }
        [[nodiscard]] inline Vector d() const { return this->corners().col(3); }

        [[nodiscard]] Box bounding_box(real slack = Pixel::Slack) const;
        [[nodiscard]] bool contains(const Vector& point) const;

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
