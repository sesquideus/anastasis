#ifndef ANASTASIS_CPP_EIGEN_H
#define ANASTASIS_CPP_EIGEN_H

#include <Eigen/Eigen>
#include "types/types.h"

namespace Astar {
    typedef Eigen::Matrix<real, 2, 2> Matrix2D;
    typedef Eigen::Matrix<real, Eigen::Dynamic, Eigen::Dynamic> Matrix;
    typedef Eigen::Translation<real, 2> Translation;
    //typedef std::conditional<std::is_same<real, double>::value, Eigen::Vector2d, Eigen::Vector2f> Vector;
    typedef Eigen::Vector2d Vector;
    typedef Eigen::Matrix<real, 2, 4> Quadrangle;
    typedef Eigen::Transform<real, 2, Eigen::AffineCompact> AffineTransform;
    typedef Eigen::SparseMatrix<real> SparseMatrix;

    SparseMatrix stack(const std::vector<SparseMatrix> & matrices, bool vertical);
    SparseMatrix vstack(const std::vector<SparseMatrix> & matrices);
    SparseMatrix hstack(const std::vector<SparseMatrix> & matrices);
    SparseMatrix vstack2(std::vector<SparseMatrix> matrices);

    // Move these to more appropriate place
    constexpr static real Slack = 1e-15;
    constexpr static real MinimumDeterminant = 1e-12;
    static Vector InvalidVector(std::numeric_limits<real>::quiet_NaN(), std::numeric_limits<real>::quiet_NaN());

    /** 2D pseudo cross product **/
    inline real operator^(const Vector & lhs, const Vector & rhs) {
        return lhs.x() * rhs.y() - lhs.y() * rhs.x();
    }

    Vector line_segment_intersection(const Vector & p0, const Vector & p1, const Vector & q0, const Vector & q1);

    bool is_valid(const Vector & vector);
}

#endif //ANASTASIS_CPP_EIGEN_H
