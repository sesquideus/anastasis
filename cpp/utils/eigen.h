#ifndef ANASTASIS_CPP_EIGEN_H
#define ANASTASIS_CPP_EIGEN_H

#include <Eigen/Eigen>
#include "types/types.h"

Eigen::SparseMatrix<real> stack(const std::vector<Eigen::SparseMatrix<real>> & matrices, bool vertical);
Eigen::SparseMatrix<real> vstack(const std::vector<Eigen::SparseMatrix<real>> & matrices);
Eigen::SparseMatrix<real> hstack(const std::vector<Eigen::SparseMatrix<real>> & matrices);
Eigen::SparseMatrix<real> vstack2(std::vector<Eigen::SparseMatrix<real>> matrices);

#endif //ANASTASIS_CPP_EIGEN_H
