#ifndef ANASTASIS_CPP_KDTREE_H
#define ANASTASIS_CPP_KDTREE_H

#include "../spatial.h"
#include "../metrics.h"

class KDTree: public Spatial<Point, Euclidean> {
public:
    explicit KDTree(const std::vector<Point> & points);

    const Distant<Point> & find_nearest(const Point & point) const override;
    std::vector<Distant<Point>> find_nearest(const Point & point, unsigned int k) const override;
    std::vector<Distant<Point>> find_radius(const Point & point, real radius) const override;
};



#endif //ANASTASIS_CPP_KDTREE_H
