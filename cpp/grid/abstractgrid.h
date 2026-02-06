#ifndef ANASTASIS_CPP_ABSTRACTGRID_H
#define ANASTASIS_CPP_ABSTRACTGRID_H

#include "types/types.h"

namespace Astar {
    /** Abstract grid with no data nor placement, just size
     *  Not really useful by itself, but a good universal common ancestor
     *  for images, placed grids and everything related.
     *  It is expected to be immutable: resizing is not allowed.
     */
    class AbstractGrid {
    private:
        int size_w_;
        int size_h_;
    public:
        explicit AbstractGrid(const int width, const int height):
            size_w_(width),
            size_h_(height) {}
        explicit AbstractGrid(const pair<int> &size):
            AbstractGrid(size.first, size.second) {}

        virtual ~AbstractGrid() = default;

        [[nodiscard]] int width() const { return this->size_w_; }
        [[nodiscard]] int height() const { return this->size_h_; }
        [[nodiscard]] pair<int> size() const { return {this->size_w_, this->size_h_}; }
        [[nodiscard]] int count() const { return this->width() * this->height(); }
    };
}

#endif //ANASTASIS_CPP_ABSTRACTGRID_H
