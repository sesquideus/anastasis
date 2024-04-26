#include "utils/eigen.h"


Eigen::SparseMatrix<real> stack(const std::vector<Eigen::SparseMatrix<real>> & matrices, bool vertical) {
    /**
     *  Stack a vector of sparse matrices, either vertically or horizontally, and produce a big sparse matrix.
     *  The other (across) dimension must always match, the other one need not be equal.
     */
    long across = 0;
    long along = 0;
    long nonzeroes = 0;
    for (auto && matrix: matrices) {
        long num_along = vertical ? matrix.rows() : matrix.cols();
        long num_across = vertical ? matrix.cols() : matrix.rows();
        if (along > 0) {
            assert((num_across == across) && "Across-concatenation dimensions do not match");
        }
        across = num_across;
        along += num_along;
        nonzeroes += matrix.nonZeros();
    }

    // fmt::print("Joining {} matrices along {} axis\n", matrices.size(), vertical ? "vertical" : "horizontal");
    std::vector<Eigen::Triplet<real>> triplets;
    triplets.reserve(nonzeroes);

    // Just concatenate the triplets over all matrices
    Eigen::Index base = 0;
    for (auto && matrix: matrices) {
        for (Eigen::Index c = 0; c < matrix.outerSize(); ++c) {
            for (Eigen::SparseMatrix<real>::InnerIterator it(matrix, c); it; ++it) {
                triplets.emplace_back(
                        vertical ? it.row() + base : it.row(),
                        vertical ? it.col() : base + it.col(),
                        it.value()
                );
            }
        }
        base += vertical ? matrix.rows() : matrix.cols();
    }

    long cols = vertical ? across : along;
    long rows = vertical ? along : across;

    Eigen::SparseMatrix<real> m(rows, cols);
    m.reserve(nonzeroes);
    fmt::print("After stacking: {}×{} matrix with {} nonzero elements\n", cols, rows, nonzeroes);

    m.setFromTriplets(triplets.begin(), triplets.end());
    return m;
}

Eigen::SparseMatrix<real> vstack(const std::vector<Eigen::SparseMatrix<real>> & matrices) {
    return stack(matrices, true);
}

Eigen::SparseMatrix<real> hstack(const std::vector<Eigen::SparseMatrix<real>> & matrices) {
    return stack(matrices, false);
}


Eigen::SparseMatrix<real> vstack2(std::vector<Eigen::SparseMatrix<real>> matrices) {
    long rows = 0;
    long cols = 0;
    long nonzeros = 0;
    for (auto & matrix: matrices) {
        if ((rows > 0) && (matrix.cols() != cols)) {
            throw std::runtime_error("Sparse matrix width does not match");
        }
        cols = matrix.cols();
        rows += matrix.rows();
        nonzeros += matrix.nonZeros();
    }

    Eigen::SparseMatrix<real> m(rows, cols);
    m.reserve(nonzeros);
    fmt::print("{} {}\n", cols, nonzeros);

    for (Eigen::Index c = 0; c < cols; ++c) {
        Eigen::Index base = 0;
        for (auto && matrix: matrices) {
            m.startVec(c);
            for (Eigen::SparseMatrix<real>::InnerIterator it(matrix, c); it; ++it) {
                matrix.startVec(c);
                fmt::print("s {} {}\n", it, c);
                m.insertBack(base + it.row(), c) = it.value();
                fmt::print("e {} {}\n", it, c);
            }
            base += matrix.rows();
        }
    }
    m.finalize();
    return m;
}

//
// Created by kvik on 26/04/24.
//

#include "eigen.h"
