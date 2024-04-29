#include <fmt/format.h>
#include <fmt/ranges.h>
#include <numeric>
#include <algorithm>
#include <vector>
#include "pixel.h"

namespace Astar {
    Pixel::Pixel():
        corners_ {{{Point(0, 0), Point(0, 0)}, {Point(0, 0), Point(0, 0)}}}
    {}

    Pixel::Pixel(const Point bottomleft, const Point bottomright, const Point topleft, const Point topright):
        corners_ {{{bottomleft, bottomright}, {topleft, topright}}}
    { }

    Box Pixel::bounding_box(real slack) const {
        /**
         * Returns a bounding box: the smallest aligned Pixel structure that is guaranteed to cover
         * the entire area of the pixel + optionally a tiny constant <slack>.
         * Useful for determining overlaps with the canonical grid.
         */
        auto xs = {this->a().x, this->b().x, this->c().x, this->d().x};
        auto ys = {this->a().y, this->b().y, this->c().y, this->d().y};
        int minx = static_cast<int>(floor(std::min(xs) - slack));
        int maxx = static_cast<int>(ceil(std::max(xs) + slack));
        int miny = static_cast<int>(floor(std::min(ys) - slack));
        int maxy = static_cast<int>(ceil(std::max(ys) + slack));

        return Box(minx, maxx, miny, maxy);
    }

    bool Pixel::contains(Point point) const {
        /** Determine whether a point is contained within the pixel
         *  Adds a small slack since there is only a tiny penalty for including a pixel
         *  that is very close to the bounds, but a catastrophic failure if a pixel
         *  is falsely reported as outside due to rounding errors.
         */
        Point p = this->b() - this->a(); // Line AB
        Point q = this->d() - this->a(); // Line AD
        Point x = point - this->a();     // Line AX
        real t = (p * x) / (p * p);      // Projection of AX onto AB
        real u = (q * x) / (q * q);      // Projection of AX onto AD
        return ((-Pixel::Slack <= t) && (t <= 1.0 + Pixel::Slack) && (-Pixel::Slack <= u) && (u <= 1.0 + Pixel::Slack));
    }

    real Pixel::area() const {
        /** Find the area of the pixel.
         *  Currently we assume that the pixel is always a parallelogram,
         *  for more advanced distortions this may not be true, update if this happens! */
        return (this->b() - this->a()) ^ (this->d() - this->a());
    }

    real Pixel::overlap(const Pixel & other) const {
        /** Compute the area of the overlap of this pixel with another pixel */
        std::vector<Point> vertices;
        vertices.reserve(8);

        // Check if any of this pixel's vertices lie within the other pixel, if so, add it as a candidate
        for (auto candidate: {this->a(), this->b(), this->c(), this->d()}) {
            if (other.contains(candidate)) {
                vertices.push_back(candidate);
            }
        }
        // Check if any of the other pixel's vertices lie within this pixel, if so, add it as a candidate
        for (auto candidate: {other.a(), other.b(), other.c(), other.d()}) {
            if (this->contains(candidate)) {
                vertices.push_back(candidate);
            }
        }

        /* Find all the intersections over the Cartesian product of this and other pixel's edges
         * The order of the evaluation is 00, 01, 10, 11 × 00, 01, 10, 11
         *
         * The index cycles through the edges of both rectangles in a clever way
         * See https://ksvi.mff.cuni.cz/~kryl/dokumentace.htm#koment, last line:
         * "Definitely add comments to lines where you are proud of how clever your solution is.
         * From my own experience it often pays off to start looking for insidious bugs right there." */
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
            // If there are less than 3 candidate vertices, there is certainly no overlap
            return 0;
        } else {
            // Otherwise find the centre of mass and move it to the origin along with all the points
            Point centre = std::accumulate(vertices.cbegin(), vertices.cend(), Point()) / static_cast<real>(vertices.size());
            for (auto && v: vertices) {
                v -= centre;
            }
            // Sort vertices by azimuth from the centre
            // TODO This can be precomputed! Sort by another vector of v.slope()
            std::sort(vertices.begin(), vertices.end(), [&](Point a, Point b) -> bool { return a.slope() < b.slope(); });

            // Run the shoelace algorithm to compute the area of the convex polygon
            real shoelace = 0;
            std::size_t size = vertices.size();
            for (std::size_t i = 0; i < size; ++i) {
                shoelace += vertices[i].x * (vertices[(i + 1) % size].y - vertices[(i - 1 + size) % size].y);
            }

            return shoelace * 0.5;
        }
    }

    real Pixel::operator&(const Pixel & other) const {
        /** Operator shorthand for overlap (pixel * other) **/
        return this->overlap(other);
    }

    real Pixel::operator|(const Pixel & other) const {
        /** Operator shorthand for area of union **/
        return this->area() + other.area() - ((*this) & other);
    }
}
