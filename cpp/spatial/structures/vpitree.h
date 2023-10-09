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

template<class T, class Metric> requires IsMetric<T, Metric>
class VPImplicitTree: public Spatial<T, Metric> {
private:
    std::vector<T> points_;  // internal storage of points
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
    explicit VPImplicitTree(const std::vector<T> & items);

    const Distant<T> & find_nearest(const T & point) const override;
    std::vector<Distant<T>> find_nearest(const T & point, unsigned int count) const override;
    std::vector<Distant<T>> find_radius(const T & point, real radius) const override;
};


long int bit_ceil(long int x) {
    static const unsigned int t[5] = {
            0xFFFF0000,
            0x0000FF00,
            0x000000F0,
            0x0000000C,
            0x00000002
    };

    int y = (((x & (x - 1)) == 0) ? 0 : 1);
    int j = 16;
    int i;

    for (i = 0; i < 5; i++) {
        int k = (((x & t[i]) == 0) ? 0 : j);
        y += k;
        x >>= k;
        j >>= 1;
    }

    return 1 << y;
}

long int bit_floor(long int x) {
    return x & (bit_ceil((x >> 1) + 1));
}

long int full_inner(long int total) {
    long int s = bit_floor(total >> 1);
    if (total < 2) {
        return 0;
    }
    if (total - (s << 1) < s) {
        return total - s;
    } else {
        return (s << 1) - 1;
    }
}
#include "vpitree.tpp"

#endif // VPITREE_H
