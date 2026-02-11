#ifndef VPITREE_H
#define VPITREE_H

#include <queue>
#include <iostream>

#include "../spatial.h"
#include "../metrics.h"

namespace Astar {
    template<typename T>
    struct VPIItem {
        const T * address;
        real threshold;

        explicit VPIItem(const T * _address):
            address(_address),
            threshold(NAN)
        {}
    };

    template<class T, class Metric> requires Astar::IsMetric<T, Metric>
    class VPImplicitTree: public Astar::Spatial<T, Metric> {
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

        void search_one(unsigned int index, const T & target, Astar::Distant<T> & candidate) const;
        void search_queued(unsigned int index, const T & target, unsigned int count, std::priority_queue<Astar::Distant<T>> & pq) const;
        void search_radius(unsigned int index, const T & target, real radius, std::vector<Astar::Distant<T>> & result) const;

        inline const T & point(unsigned int index) const { return *this->array_[index].address; };
        //const T & point(unsigned int index) const;

        void print() const override;
        void print(unsigned int root, unsigned int offset) const;

    public:
        explicit VPImplicitTree(const std::vector<T> & items);

        const Astar::Distant<T> & find_nearest(const T & point) const override;
        std::vector<Astar::Distant<T>> find_nearest(const T & point, unsigned int count) const override;
        std::vector<Astar::Distant<T>> find_radius(const T & point, real radius) const override;
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

    template<class T, class Metric> requires Astar::IsMetric<T, Metric>
    VPImplicitTree<T, Metric>::VPImplicitTree(const std::vector<T> & items):
            Spatial<T, Metric>(),
            points_(items)
    {
        this->size_ = items.size();
        this->heap_.resize(items.size());
        std::vector<T*> pointers(items.size(), nullptr);

        for (unsigned int i = 0; i < this->size_; ++i) {
            pointers[i] = &(this->points_[i]);
        }

        this->build(0, pointers.begin(), pointers.begin(), pointers.end());
    }

    template<class T, class Metric> requires IsMetric<T, Metric>
    void VPImplicitTree<T, Metric>::build(
            unsigned int index,
            typename std::vector<T*>::iterator begin,
            typename std::vector<T*>::iterator root,
            typename std::vector<T*>::iterator end)
    {
        if (begin >= end) {
            // if there are no more nodes to process, make this a leaf node
            if (index < this->size_) {
                this->heap_[index].threshold = 0.0;
            }
        } else {
            if (begin == end - 1) {
                // if there is exactly one node to process, make it a leaf node
                this->heap_[index].address = *begin;
                this->heap_[index].threshold = 0.0;
            } else {
                // otherwise there is at least one child
                // build a comparator function with respect to the vantage point
                auto comparator_root = [&](const T * first, const T * second) -> bool {
                    return this->metric_(**root, *first) < this->metric_(**root, *second);
                };
                // the pivot is the element furthest from the vantage point
                auto pivot = std::max_element(begin, end, comparator_root);
                // swap the pivot to the beginning
                std::iter_swap(begin, pivot);
                // create an iterator pointing to where the inner-outer division should be
                auto median = begin + full_inner(end - begin) + 1;
                // partition the interval with respect to the division (which is now at begin)
                auto comparator_pivot = [&](const T * first, const T * second) -> bool {
                    return this->metric_(**begin, *first) < this->metric_(**begin, *second);
                };
                std::nth_element(begin, median, end, comparator_pivot);

                // add the new node and recursively process the subintervals
                this->heap_[index].address = *begin;
                this->heap_[index].threshold = this->metric_(**begin, **median);
                this->build(2 * index + 1, begin + 1, begin, median);
                this->build(2 * index + 2, median, begin, end);
            }
        }
    }

    /*
    template<class T, class U>
    void VPImplicitTree<T, U>::load(const std::string & filename) {
        try {
            std::ifstream in(filename);
            in.read(reinterpret_cast<char *>(&this->size_), sizeof(this->size_));
            this->points_.reserve(this->size_);
            in.read(reinterpret_cast<char *>(this->points_.data()), this->size_ * sizeof(T));
            this->heap_.reserve(this->size_);
            in.read(reinterpret_cast<char *>(this->heap_.data()), this->size_ * sizeof(T));
        } catch (const std::exception & e) {
            throw e;
        }
    }

    template<class T, class U>
    void VPImplicitTree<T, U>::save(const std::string & filename) const {
        std::ofstream out(filename);
        out.write(reinterpret_cast<const char *>(this->size_), sizeof(this->size_));
        out.write(reinterpret_cast<const char *>(this->points_.data()), this->size_ * sizeof(T));
        out.write(reinterpret_cast<const char *>(this->heap_.data()), this->size_ * sizeof(T));
    }*/

    template<class T, class Metric> requires IsMetric<T, Metric>
    const Distant<T> & VPImplicitTree<T, Metric>::find_nearest(const T & point) const {
        Distant<T> candidate(this->points_[0], Metric(this->points_[0], point));
        this->tau_ = std::numeric_limits<real>::max();

        this->search_one(0, point, candidate);
        return candidate;
    }

    template<class T, class Metric> requires IsMetric<T, Metric>
    std::vector<Distant<T>> VPImplicitTree<T, Metric>::find_nearest(const T & point, unsigned int count) const {
        std::priority_queue<Distant<T>> pq;
        std::vector<Distant<T>> result;

        this->tau_ = std::numeric_limits<real>::max();
        this->search_queued(0, point, count, pq);

        while (!pq.empty()) {
            result.push_back(pq.top());
            pq.pop();
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    template<class T, class Metric> requires IsMetric<T, Metric>
    std::vector<Distant<T>> VPImplicitTree<T, Metric>::find_radius(const T & point, real radius) const {
        std::vector<Distant<T>> result;
        this->search_radius(0, point, radius, result);
        return result;
    }

    /** Search for a single point that is nearest to <target> starting at <node>
     *  Functionally the same as search_queued with k = 1 but slightly faster
     * **/
    template<class T, class Metric> requires IsMetric<T, Metric>
    void VPImplicitTree<T, Metric>::search_one(
        unsigned int index,
        const T & target,
        Distant<T> & candidate
    ) const {
        if (index < this->size_) {
            real distance = this->metric_(this->point(index), target);

            if (distance < this->tau_) {
                candidate = Distant<T>(this->point(index), distance);
                this->tau_ = distance;
            }

            real threshold = this->heap_[index].threshold;
            if (distance < threshold) {
                this->search_one(2 * index + 1, target, candidate);
                if (distance + this->tau_ >= threshold) {
                    this->search_one(2 * index + 2, target, candidate);
                }
            } else {
                this->search_one(2 * index + 2, target, candidate);
                if (distance - this->tau_ <= threshold) {
                    this->search_one(2 * index + 1, target, candidate);
                }
            }
        }
    }

    /* Search for k nearest points to <target>, starting at <node> */
    template<class T, class Metric> requires IsMetric<T, Metric>
    void VPImplicitTree<T, Metric>::search_queued(
        unsigned int index,
        const T & target,
        unsigned int count, std::priority_queue<Distant<T>> & pq
    ) const {
        if (index < this->size_) {
            real distance = this->metric_(this->point(index), target);

            if (distance < this->tau_) {
                if (pq.size() == count) {
                    pq.pop();
                }
                pq.push(Distant<T>(this->point(index), distance));
                if (pq.size() == count) {
                    this->tau_ = pq.top().distance;
                }
            }

            real threshold = this->heap_[index].threshold;
            if (2 * index < this->size_) {
                if (distance < threshold) {
                    this->search_queued(2 * index + 1, target, count, pq);
                    if (distance + this->tau_ >= threshold) {
                        this->search_queued(2 * index + 2, target, count, pq);
                    }
                } else {
                    this->search_queued(2 * index + 2, target, count, pq);
                    if (distance - this->tau_ <= threshold) {
                        this->search_queued(2 * index + 1, target, count, pq);
                    }
                }
            }
        }
    }

    /* Find all points that are closer to <target> than <radius> */
    template<class T, class Metric> requires IsMetric<T, Metric>
    void VPImplicitTree<T, Metric>::search_radius(
        unsigned int index,
        const T & target,
        real radius,
        std::vector<Distant<T>> & result
    ) const {
        if (index < this->size_) {
            real distance = this->metric_(this->point(index), target);

            if (distance < radius) {
                result.push_back(Distant<T>(this->point(index), distance));
            }

            real threshold = this->heap_[index].threshold;
            if (distance < threshold) {
                this->search_radius(2 * index + 1, target, radius, result);
                if (distance + radius >= threshold) {
                    this->search_radius(2 * index + 2, target, radius, result);
                }
            } else {
                this->search_radius(2 * index + 2, target, radius, result);
                if (distance - radius <= threshold) {
                    this->search_radius(2 * index + 1, target, radius, result);
                }
            }
        }
    }

    template<class T, class Metric> requires IsMetric<T, Metric>
    void VPImplicitTree<T, Metric>::print() const {
        this->print(0, 0);

        std::cout << std::endl;
        for (unsigned int i = 0; i < this->size_; ++i) {
            std::cout << i << " points to " << this->heap_[i].address << ", value " << this->point(i);
            std::cout << " (threshold " << this->heap_[i].threshold << ")" << std::endl;
        }
        std::cout << "----------------------------------------------" << std::endl;
    }

    template<class T, class Metric> requires IsMetric<T, Metric>
    void VPImplicitTree<T, Metric>::print(unsigned int root, unsigned int offset) const {
        if (root < this->size_) {
            this->print(root * 2 + 1, offset + 1);
            for (unsigned int i = 0; i < offset; ++i) {
                std::cout << "    ";
            }
            std::cout << root << "(" << this->heap_[root].address << ") " << this->heap_[root].threshold << std::endl;
            this->print(root * 2 + 2, offset + 1);
        } else {
            return;
        }
    }

    /*
    template<class T, class Metric> requires IsMetric<T, Metric>
    void VPImplicitTree<T, Metric>::print_points(void) const {
        for (unsigned int i = 0; i < this->size_; ++i) {
            std::cout << this->points_[i] << " ";
        }
        std::cout << std::endl;
    }
    */
}
#endif // VPITREE_H