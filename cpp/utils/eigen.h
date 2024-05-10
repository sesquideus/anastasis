#ifndef ANASTASIS_CPP_EIGEN_H
#define ANASTASIS_CPP_EIGEN_H

#include <Eigen/Eigen>
#include "types/types.h"

namespace Astar {
    typedef Eigen::Matrix<real, 2, 2> Matrix2D;
    typedef Eigen::Matrix<real, Eigen::Dynamic, Eigen::Dynamic> Matrix;
    typedef Eigen::Transform<real, 2, Eigen::AffineCompact> AffineTransform;
    typedef Eigen::SparseMatrix<real> SparseMatrix;

    Matrix2D rotation_matrix(real rotation);
    Matrix2D scaling_matrix(pair<real> scale);

    SparseMatrix stack(const std::vector<SparseMatrix> & matrices, bool vertical);
    SparseMatrix vstack(const std::vector<SparseMatrix> & matrices);
    SparseMatrix hstack(const std::vector<SparseMatrix> & matrices);
    SparseMatrix vstack2(std::vector<SparseMatrix> matrices);
}

#endif //ANASTASIS_CPP_EIGEN_H
