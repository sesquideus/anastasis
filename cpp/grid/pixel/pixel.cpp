#include <fmt/format.h>
#include <fmt/ranges.h>
#include <numeric>
#include <algorithm>
#include <utility>
#include <vector>

#include "pixel.h"
#include "utils/eigen.h"

namespace Astar {
    Pixel::Pixel():
        corners_((Quadrangle() << 0, 0, 0, 0, 0, 0, 0, 0).finished())
    {}

    Pixel::Pixel(const Vector & bottom_left, const Vector & bottom_right,
                 const Vector & top_left, const Vector & top_right):
        corners_((Quadrangle() << bottom_left, bottom_right, top_left, top_right).finished())
    {}

    Pixel::Pixel(Quadrangle quadrangle):
        corners_(std::move(quadrangle))
    {}

    /** Construct from a Box type
     * @param box
     */
    Pixel::Pixel(const Box & box):
        Pixel(
            Vector(static_cast<real>(box.left), static_cast<real>(box.bottom)),
            Vector(static_cast<real>(box.right), static_cast<real>(box.bottom)),
            Vector(static_cast<real>(box.left), static_cast<real>(box.top)),
            Vector(static_cast<real>(box.right), static_cast<real>(box.top))
        )
    { }

    Box Pixel::bounding_box(real slack) const {
        /**
         * Returns a bounding box: the smallest aligned Pixel structure that is guaranteed to cover
         * the entire area of the pixel + optionally a tiny constant <slack>.
         * Useful for determining overlaps with the canonical grid.
         */
        auto xs = {this->a().x(), this->b().x(), this->c().x(), this->d().x()};
        auto ys = {this->a().y(), this->b().y(), this->c().y(), this->d().y()};
        int minx = static_cast<int>(floor(std::min(xs) - slack));
        int maxx = static_cast<int>(ceil(std::max(xs) + slack));
        int miny = static_cast<int>(floor(std::min(ys) - slack));
        int maxy = static_cast<int>(ceil(std::max(ys) + slack));

        return Box(minx, maxx, miny, maxy);
    }

    bool Pixel::contains(const Vector & point) const {
        /** Determine whether a point is contained within the pixel
         *  Adds a small slack since there is only a tiny penalty for including a pixel
         *  that is very close to the bounds, but a catastrophic failure if a pixel
         *  is falsely reported as outside due to rounding errors.
         */
        Vector p = this->b() - this->a(); // Line AB
        Vector q = this->d() - this->a(); // Line AD
        Vector x = point - this->a();     // Line AX
        real t = (p.dot(x)) / p.squaredNorm();      // Projection of AX onto AB
        real u = (q.dot(x)) / q.squaredNorm();      // Projection of AX onto AD
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
        std::vector<Vector> vertices;
        vertices.reserve(8);

        // Check if any of this pixel's vertices lie within the other pixel, if so, add it as a candidate
        for (const auto & candidate: {this->a(), this->b(), this->c(), this->d()}) {
            if (other.contains(candidate)) {
                vertices.push_back(candidate);
            }
        }
        // Check if any of the other pixel's vertices lie within this pixel, if so, add it as a candidate
        for (const auto & candidate: {other.a(), other.b(), other.c(), other.d()}) {
            if (this->contains(candidate)) {
                vertices.push_back(candidate);
            }
        }

        /* Find all the intersections over the Cartesian product of this and other pixel's edges
         * The indices cycle through the edges of both rectangles in a clever way
         * See https://ksvi.mff.cuni.cz/~kryl/dokumentace.htm#koment, last line:
         * "Definitely add comments to lines where you are proud of how clever your solution is.
         * From my own experience it often pays off to start looking for insidious bugs right there." */
        for (unsigned char i = 0; i < 4; ++i) {
            for (unsigned char j = 0; j < 4; ++j) {
                auto point = line_segment_intersection(this->corners().col(i % 4),
                                                       this->corners().col((i + 1) % 4),
                                                       other.corners().col(j % 4),
                                                       other.corners().col((j + 1) % 4));
                if (is_valid(point)) {
                    vertices.push_back(point);
                }
            }
        }

        if (vertices.size() < 3) {
            // If there are less than 3 candidate vertices, there is certainly no overlap with area
            return 0;
        } else {
            // Otherwise find the centre of mass and move it to the origin along with all the points
            Vector centre = std::accumulate(vertices.cbegin(), vertices.cend(), Vector(0, 0)) / static_cast<real>(vertices.size());
            for (auto && v: vertices) {
                v -= centre;
            }
            // Sort vertices by azimuth from the centre
            // TODO This can be precomputed! Sort by another vector of v.slope()
            std::sort(vertices.begin(), vertices.end(),
                      [&](const Vector & a, const Vector & b) -> bool {
                          return std::atan2(a.y(), a.x()) < std::atan2(b.y(), b.x());
                      });

            // Run the shoelace algorithm to compute the area of the convex polygon
            real shoelace = 0;
            std::size_t size = vertices.size();
            for (std::size_t i = 0; i < size; ++i) {
                shoelace += vertices[i].x() * (vertices[(i + 1) % size].y() - vertices[(i - 1 + size) % size].y());
            }

            return shoelace * 0.5;
        }
    }

    /** Orthogonal overlap works only if pixel boundaries are aligned with the grid
     * @param other
     * @return real
     */
    real Pixel::orthogonal_overlap(const Pixel & other) const {
        auto ortho_me = Pixel(this->bounding_box());
        auto ortho_other = Pixel(other.bounding_box());
        return linear_overlap({ortho_me.b().x(), ortho_me.a().x()},
                              {ortho_other.b().x(), ortho_other.a().x()}) *
               linear_overlap({ortho_me.d().y(), ortho_me.a().y()},
                              {ortho_other.d().y(), ortho_other.a().y()});
    }

    real Pixel::operator&(const Pixel & other) const {
        return this->overlap(other);
    }

    real Pixel::operator|(const Pixel & other) const {
        return this->area() + other.area() - ((*this) & other);
    }
}

real linear_overlap(pair<real> int1, pair<real> int2) {
    real c1 = (int1.first + int1.second) / 2;
    real c2 = (int2.first + int2.second) / 2;
    real w1 = (int1.second - int1.first) / 2;
    real w2 = (int2.second - int2.first) / 2;
    real delta = std::abs(c2 - c1);
    if (delta <= std::abs(w2 - w1)) {
        return 0;
    } else if (delta <= std::abs(w1 + w2)) {
        return (std::abs(w1 + w2) - delta) / w2;
    } else {
        return std::min(w1, w2);
    }
}
