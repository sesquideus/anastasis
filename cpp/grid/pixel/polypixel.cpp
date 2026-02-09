#include <algorithm>
#include "polypixel.h"

#include <numeric>

#include "pixel.h"

namespace Astar {
    bool triangle_zero_contains(const Point & p, const Point & q, const Point & x) {
        /* Determine whether the triangle 0-p-q contains point x. */
        const real s = (p * x) / (p * p) + (q * x) / (q * q);
        const real prod = (p ^ x) * (q ^ x);
        /* The point is inside the triangle if
           - it's in opposite half-planes with respect to p and q,
           - projects positively onto both vectors,
           - and is not further than the convex combination line.

        Some slack is included since false positives only change the result a tiny bit,
        but while false negatives are catastrophic. */
        return (-Pixel::Slack <= s) && (s <= 1 + Pixel::Slack) && (prod <= Pixel::Slack);
    }

    bool triangle_contains(const Point & o, const Point & p, const Point & q, const Point & x) {
        // Determine whether the triangle o-p-q contains point x.
        // Calls the zero-based version internally.
        return triangle_zero_contains(p - o, q - o, x - o);
    }

    const Point & PolyPixel::operator[](const int index) const {
        /* Quick access operator to vertices.
         * Supports cycling indexing (..., -2, -1, 0, 1, 2, ... N - 2, N - 1, N = 0, N + 1 = 1, ...) */
        const int x = index % this->degree();
        return this->_vertices[x < 0 ? this->degree() + x : x];
    }

    Point PolyPixel::centre() const {
        /* Find the centre of the polygon -- not necessarily the centre of mass,
         * but a point which is guaranteed to lie inside, as long as the polygon is non-degenerate.
         * Such a point is great for finding the area or such applications. */
        if (this->degree() == 0) {
            return {0, 0};
        }
        return std::accumulate(this->_vertices.cbegin(), this->_vertices.cend(), Point(0, 0)) / this->degree();
    }

    real PolyPixel::area() const {
        auto zeroed = this->to_zero();
        // Run the shoelace algorithm to compute the area of the convex polygon
        if (this->degree() < 3) {
            return 0;
        } else {
            real shoelace = 0;
            for (int i = 0; i < this->degree(); ++i) {
                shoelace += this->_vertices[i].x * ((*this)[i + 1].y - (*this)[i - 1].y);
            }
            return shoelace * 0.5;
        }
    }

    PolyPixel PolyPixel::to_zero() const {
        PolyPixel result(*this);
        std::ranges::for_each(result._vertices, [&](const Point x) -> Point { return x - this->_vertices[0]; });
        return result;
    }

    bool PolyPixel::contains(const Point & point) const {
        for (int i = 1; i < this->degree() - 1; ++i) {
            if (triangle_contains((*this)[0], (*this)[i], (*this)[i + 1], point)) {
                return true;
            }
        }
        return false;
    }

    PolyPixel operator & (const PolyPixel & first, const PolyPixel & second) {
        std::vector<Point> candidates;
        candidates.reserve(first.degree() + second.degree());

        // Check if any of this pixel's vertices lie within the other pixel, if so, add it as a candidate
        for (const auto & candidate: first._vertices) {
            if (second.contains(candidate)) {
                candidates.push_back(candidate);
            }
        }
        // Check if any of the other pixel's vertices lie within this pixel, if so, add it as a candidate
        for (const auto & candidate: second._vertices) {
            if (first.contains(candidate)) {
                candidates.push_back(candidate);
            }
        }

        // Find all the intersections over the Cartesian product of this and another pixel's edges
        for (int i = 0; i < first.degree(); ++i) {
            for (int j = 0; j < second.degree(); ++j) {
                if (auto point = Point::line_segment_intersection(first[i], first[i + 1], second[j], second[j + 1]);
                    point.is_valid()
                ) {
                    candidates.push_back(point);
                }
            }
        }

        if (candidates.size() < 3) {
            // If there are less than 3 candidate vertices, there is certainly no overlap.
            // Return the degenerate PolyPixel anyway.
            return PolyPixel(candidates);
        } else {
            // Otherwise find the centre of mass and move it to the origin along with all the points
            const Point centre = std::accumulate(candidates.cbegin(), candidates.cend(), Point(0, 0)) / static_cast<real>(candidates.size());
            // Sort vertices by azimuth from the centre
            std::ranges::sort(candidates, [&](const Point a, const Point b) -> bool {
                return (a - centre).slope() < (b - centre).slope();
            });
            return PolyPixel(candidates);
        }
    }
}
