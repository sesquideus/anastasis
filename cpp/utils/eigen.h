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
    typedef Eigen::Matrix<real, 2, 4> Parallelogram;
    typedef Eigen::Transform<real, 2, Eigen::AffineCompact> AffineTransform;
    typedef Eigen::SparseMatrix<real> SparseMatrix;

    SparseMatrix stack(const std::vector<SparseMatrix> & matrices, bool vertical);
    SparseMatrix vstack(const std::vector<SparseMatrix> & matrices);
    SparseMatrix hstack(const std::vector<SparseMatrix> & matrices);
    SparseMatrix vstack2(std::vector<SparseMatrix> matrices);

    /** 2D pseudo cross product **/
    inline real operator^(const Vector & lhs, const Vector & rhs) {
        return lhs.x() * rhs.y() - lhs.y() * rhs.x();
    }
}

#endif //ANASTASIS_CPP_EIGEN_H
