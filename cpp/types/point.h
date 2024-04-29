#ifndef ANASTASIS_CPP_POINT_H
#define ANASTASIS_CPP_POINT_H

#include <cmath>

#include "types.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>

namespace Astar {
    constexpr real Invalid = std::numeric_limits<real>::quiet_NaN();

    class Point {
    private:
        constexpr static real Slack = 1e-15;
        constexpr static real MinimumDeterminant = 1e-12;
    public:
        real x;
        real y;
        [[nodiscard]] inline bool is_valid() const { return !(std::isnan(this->x) || std::isnan(this->y)); };
        [[nodiscard]] Point rotated(real angle) const;
        [[nodiscard]] real slope() const;

        Point(real x, real y);

        /** Unary minus for vectors (default) **/
        Point operator-() const;
        Point & operator+=(Point shift);
        Point & operator-=(Point shift);
        Point & operator*=(real scale);
        Point & operator/=(real scale);

        inline static Point invalid() { return Point(Invalid, Invalid); };
        static Point line_segment_intersection(Point p0, Point p1, Point q0, Point q1);
    };

    inline real distance_euclidean(const Point p1, const Point p2) {
        real dx = p2.x - p1.x;
        real dy = p2.y - p1.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    // Addition of two points: sum of coordinates
    inline Point operator+(Point first, Point second) {
        return first += second;
    }

    // Subtraction of two points: effectively a new difference vector
    inline Point operator-(Point first, Point second) {
        return Point(first.x - second.x, first.y - second.y);
    }

    // Multiplication by a scalar: multiplication of a vector
    inline Point operator*(real scale, Point point) {
        return Point(scale * point.x, scale * point.y);
    }

    // Inverse order of the same
    inline Point operator*(Point point, real scale) {
        return Point(scale * point.x, scale * point.y);
    }

    // Division by a scalar: effectively shortening of a vector
    inline Point operator/(Point point, real scale) {
        return Point(point.x / scale, point.y / scale);
    }

    // Dot product
    inline real operator*(Point first, Point second) {
        return first.x * second.x + first.y * second.y;
    }

    // Pseudo cross product of two vectors: a scalar z representing (0, 0, z) in 3D
    inline real operator^(Point first, Point second) {
        return first.x * second.y - first.y * second.x;
    }

    // OK this is probably a bad idea... violates the principle of least surprise
    inline real operator<<(Point first, Point second) { return distance_euclidean(first, second); }
    inline real operator>>(Point first, Point second) { return distance_euclidean(first, second); }
}

template<>
class fmt::formatter<Astar::Point> {
private:
    char presentation = 'f';

public:
    constexpr auto parse(fmt::format_parse_context & ctx) -> fmt::format_parse_context::iterator {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;
        if (it != end && *it != '}') return it;
        return it;
    }

    auto format(const Astar::Point & point, format_context & ctx) const -> format_context::iterator {
        return presentation == 'f'
               ? fmt::format_to(ctx.out(), "({:.6f}, {:.6f})", point.x, point.y)
               : fmt::format_to(ctx.out(), "({:.6e}, {:.6e})", point.x, point.y);
    }
};

#endif //ANASTASIS_CPP_POINT_H