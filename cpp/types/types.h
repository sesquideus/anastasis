#ifndef ANASTASIS_CPP_TYPES_H
#define ANASTASIS_CPP_TYPES_H

#include <Eigen/Sparse>

typedef double real;

template<class T>
using pair = std::pair<T, T>;

typedef std::tuple<long, long, long, long, real> Overlap4D;
typedef Eigen::Matrix<real, Eigen::Dynamic, Eigen::Dynamic> Matrix;



#endif //ANASTASIS_CPP_TYPES_H
