#ifndef ANASTASIS_CPP_KDTREE_H
#define ANASTASIS_CPP_KDTREE_H

#include "../spatial.h"
#include "../metrics.h"

template<class T>
class KDTree: public Spatial<Point, Euclidean> {
public:
    explicit KDTree(const std::vector<Point> & points);

    const Distant<T> & find_nearest(const T & point) const override;
    std::vector<Distant<T>> find_nearest(const T & point, unsigned int k) const override;
    std::vector<Distant<T>> find_radius(const T & point, real radius) const override;
};



#endif //ANASTASIS_CPP_KDTREE_H
