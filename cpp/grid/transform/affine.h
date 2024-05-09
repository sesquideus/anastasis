#ifndef ANASTASIS_CPP_AFFINE_H
#define ANASTASIS_CPP_AFFINE_H

#include "utils/eigen.h"
#include "types/types.h"
#include "types/point.h"

namespace Astar {
    class Transformation {
    public:
    };

    class AffineTransformation: public Transformation {
    private:
        Point translation_;
        Matrix2D linear_;
    public:
        AffineTransformation(Point translation, pair<real> scale, real rotation);
        AffineTransformation(Point translation, const Matrix2D & matrix);

        [[nodiscard]] inline const Point & translation() const { return this->translation_; }
        [[nodiscard]] inline const Matrix2D & linear() const { return this->linear_; }
    };
}

template<>
class fmt::formatter<Astar::AffineTransformation> {
public:
    constexpr auto parse(fmt::format_parse_context & ctx) -> fmt::format_parse_context::iterator {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}') return it;
        return it;
    }

    auto format(const Astar::AffineTransformation & transform, format_context & ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(), "({}, {})", transform.translation(), transform.linear().data()[0]);
    }
};

#endif //ANASTASIS_CPP_AFFINE_H
