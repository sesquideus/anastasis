#ifndef ANASTASIS_CPP_BALLTREE_H
#define ANASTASIS_CPP_BALLTREE_H

#include "../spatial.h"

template<class T, class U>
class BallTree: public Spatial<T, U> {
public:
    explicit BallTree(const std::vector<T> & points);

    const Distant<T> & find_nearest(const T & point) const override;
    std::vector<Distant<T>> find_nearest(const T & point, unsigned int k) const override;
    std::vector<Distant<T>> find_radius(const T & point, real radius) const override;
};


#endif //ANASTASIS_CPP_BALLTREE_H
