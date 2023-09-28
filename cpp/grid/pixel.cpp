//
// Created by kvik on 9/25/23.
//

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <numeric>
#include "pixel.h"

Pixel::Pixel():
        corners_ {{{Point(0, 0), Point(0, 0)}, {Point(0, 0), Point(0, 0)}}}
{}

Pixel::Pixel(const Point bottomleft, const Point bottomright, const Point topleft, const Point topright):
        corners_ {{{bottomleft, bottomright}, {topleft, topright}}}
{ }

void Pixel::print() const {
    fmt::print("Pixel at {:9.6f} {:9.6f} -- {:9.6f} {:9.6f} -- {:9.6f} {:9.6f} -- {:9.6f} {:9.6f}\n",
               this->corners_[0][0].x, this->corners_[0][0].y,
               this->corners_[0][1].x, this->corners_[0][1].y,
               this->corners_[1][0].x, this->corners_[1][0].y,
               this->corners_[1][1].x, this->corners_[1][1].y
    );
}

Box Pixel::bounding_box(real slack) const {
    /**
     * Returns a bounding box: the smallest aligned Pixel structure that is guaranteed to cover
     * the entire area of the pixel + optionally a tiny constant <slack>.
     * Useful for determining overlaps with the canonical grid.
     */
    auto xs = {this->corners_[0][0].x, this->corners_[0][1].x, this->corners_[1][0].x, this->corners_[1][1].x};
    auto ys = {this->corners_[0][0].y, this->corners_[0][1].y, this->corners_[1][0].y, this->corners_[1][1].y};
    int minx = static_cast<int>(floor(std::min(xs) - slack));
    int maxx = static_cast<int>(ceil(std::max(xs) + slack));
    int miny = static_cast<int>(floor(std::min(ys) - slack));
    int maxy = static_cast<int>(ceil(std::max(ys) + slack));

    return Box(minx, maxx, miny, maxy);
}

bool Pixel::contains(Point point) const {
    Point p = this->corners_[0][1] - this->corners_[0][0];
    Point q = this->corners_[1][0] - this->corners_[0][0];
    Point x = point - this->corners_[0][0];
    real t = p * x / (p * p);
    real u = q * x / (q * q);
    return ((-SLACK <= t) && (t <= 1.0 + SLACK) && (-SLACK <= u) && (u <= 1.0 + SLACK));
}

real Pixel::area() const {
    return (this->corners_[0][1] - this->corners_[0][0]) ^ (this->corners_[1][0] - this->corners_[0][0]);
}

real Pixel::overlap(const Pixel & other) const {
    std::vector<Point> vertices;
    vertices.reserve(8);

    /* Check if any of this pixel's vertices lie within the other pixel */
    for (auto candidate: {this->a(), this->b(), this->c(), this->d()}) {
        if (other.contains(candidate)) {
            vertices.push_back(candidate);
        }
    }
    /* Check if any of the other pixel's vertices lie within this pixel */
    for (auto candidate: {other.a(), other.b(), other.c(), other.d()}) {
        if (this->contains(candidate)) {
            vertices.push_back(candidate);
        }
    }

    /* Find all the intersections over the Cartesian product of this and other pixel's edges */
    for (unsigned char i = 0; i < 16; ++i) {
        auto point = Point::line_segment_intersection(this->corners_[(i & 2) >> 1][((i + 1) & 2) >> 1],
                                                      this->corners_[((i + 1) & 2) >> 1][((i + 2) & 2) >> 1],
                                                      other.corners_[(i & 8) >> 3][((i + 4) & 8) >> 3],
                                                      other.corners_[((i + 4) & 8) >> 3][((i + 8) & 8) >> 3]);
        if (point.is_valid()) {
            vertices.push_back(point);
        }
    }

    if (vertices.size() < 3) {
        /* If there are less than 3 vertices, there is no overlap */
        return 0;
    } else {
        Point centre = std::accumulate(vertices.cbegin(), vertices.cend(), Point()) / static_cast<real>(vertices.size());
        for (auto && v: vertices) {
            v -= centre;
        }
        /* Sort vertices by azimuth from the centre */
        // TODO This can be precomputed! Sort by another vector of v.slope()
        std::sort(vertices.begin(), vertices.end(), [&](Point a, Point b) -> bool { return a.slope() < b.slope(); });

        /* Run the shoelace algorithm */
        real shoelace = 0;
        std::size_t size = vertices.size();
        for (std::size_t i = 0; i < size; ++i) {
            shoelace += vertices[i].x * (vertices[(i + 1) % size].y - vertices[(i - 1 + size) % size].y);
        }

        return shoelace * 0.5;
    }
}
