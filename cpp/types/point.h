#ifndef ANASTASIS_CPP_POINT_H
#define ANASTASIS_CPP_POINT_H

#include <cmath>

#include "types.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>

constexpr real INVALID = std::numeric_limits<real>::quiet_NaN();

class Point {
private:
    constexpr static real SLACK = 1e-15;
public:
    real x {0};
    real y {0};
    [[nodiscard]] inline bool is_valid() const { return !(std::isnan(this->x) || std::isnan(this->y)); };
    [[nodiscard]] Point rotated(real angle) const;
    [[nodiscard]] real slope() const;

    Point operator-() const;
    Point & operator+=(Point shift);
    Point & operator-=(Point shift);
    Point & operator*=(real scale);
    Point & operator/=(real scale);

    inline static Point invalid() { return Point(INVALID, INVALID); };
    static Point line_segment_intersection(const Point p0, const Point p1, const Point q0, const Point q1);
};

inline real distance_euclidean(const Point p1, const Point p2) {
    real dx = p2.x - p1.x;
    real dy = p2.y - p1.y;
    return std::sqrt(dx * dx + dy * dy);
}

inline Point operator+(Point first, Point second) { return Point(first.x + second.x, first.y + second.y); }
inline Point operator-(Point first, Point second) { return Point(first.x - second.x, first.y - second.y); }
inline Point operator*(real scale, Point point) { return Point(scale * point.x, scale * point.y); }
inline Point operator*(Point point, real scale) { return Point(scale * point.x, scale * point.y); }
inline Point operator/(Point point, real scale) { return Point(point.x / scale, point.y / scale); }
inline real operator*(Point first, Point second) { return first.x * second.x + first.y * second.y; }
inline real operator^(Point first, Point second) { return first.x * second.y - first.y * second.x; }

// OK this is probably a bad idea... violates the principle of least surprise
inline real operator<<(Point first, Point second) { return distance_euclidean(first, second); }
inline real operator>>(Point first, Point second) { return distance_euclidean(first, second); }


inline real dot(const Point p1, const Point p2) {
    return p1.x * p2.x + p1.y * p2.y;
}

inline real cross(const Point p1, const Point p2) {
    return p1.x * p2.y - p2.x * p1.y;
}


template<>
class fmt::formatter<Point> {
private:
    char presentation = 'f';

public:
    constexpr auto parse(fmt::format_parse_context & ctx) -> fmt::format_parse_context::iterator {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;
        if (it != end && *it != '}') return it;
        return it;
    }

    auto format(const Point & point, format_context & ctx) const -> format_context::iterator {
        return presentation == 'f'
               ? fmt::format_to(ctx.out(), "({:.6f}, {:.6f})", point.x, point.y)
               : fmt::format_to(ctx.out(), "({:.6e}, {:.6e})", point.x, point.y);
    }
};

#endif //ANASTASIS_CPP_POINT_H
