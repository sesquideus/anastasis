#ifndef ANASTASIS_CPP_TYPES_H
#define ANASTASIS_CPP_TYPES_H

#include <Eigen/Sparse>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

typedef double real;

template<class T>
using pair = std::pair<T, T>;

typedef std::tuple<long, long, long, long, real> Overlap4D;
typedef Eigen::Matrix<real, Eigen::Dynamic, Eigen::Dynamic> Matrix;
typedef Eigen::SparseMatrix<real> SparseMatrix;

// Simply tau = 2 * pi
constexpr real Tau = 3.14159265358979232846264 * 2.0;

template<class T>
class fmt::formatter<pair<T>> {
public:
    constexpr auto parse(fmt::format_parse_context & ctx) -> fmt::format_parse_context::iterator {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}') return it;
        return it;
    }

    auto format(const pair<T> & pair, format_context & ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(), "({}, {})", pair.first, pair.second);
    }
};

#endif //ANASTASIS_CPP_TYPES_H
