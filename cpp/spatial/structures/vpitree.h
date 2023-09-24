#ifndef VPITREE_H
#define VPITREE_H

#include <queue>

#include "../spatial.h"
#include "../metrics.h"

template<typename T>
struct VPIItem {
    const T * address;
    real threshold;

    explicit VPIItem(const T * _address):
        address(_address),
        threshold(NAN)
    {}
};


template<class T, class U>
class VPImplicitTree: public Spatial<T, U> {
private:
    const std::vector<T> points_;  // internal storage of points
    std::vector<VPIItem<T>> heap_;

    mutable real tau_ = 0;

    void build(
            unsigned int index,
            typename std::vector<T*>::iterator begin,
            typename std::vector<T*>::iterator root,
            typename std::vector<T*>::iterator end
    );

    void search_one(unsigned int index, const T & target, Distant<T> & candidate) const;
    void search_queued(unsigned int index, const T & target, unsigned int count, std::priority_queue<Distant<T>> & pq) const;
    void search_radius(unsigned int index, const T & target, real radius, std::vector<Distant<T>> & result) const;

    inline const T & point(unsigned int index) const { return *this->array_[index].address; };
    //const T & point(unsigned int index) const;

    void print() const override;
    void print(unsigned int root, unsigned int offset) const;

public:
    VPImplicitTree(const MetricFunctor<T> & metric, const std::vector<T> & items);

    const Distant<T> & find_nearest(const T & point) const override;
    std::vector<Distant<T>> find_nearest(const T & point, unsigned int count) const override;
    std::vector<Distant<T>> find_radius(const T & point, real radius) const override;
};

#include "vpitree.tpp"

#endif // VPITREE_H
