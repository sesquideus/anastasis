#ifndef ANASTASIS_CPP_ABSTRACTGRID_H
#define ANASTASIS_CPP_ABSTRACTGRID_H

#include "types/types.h"

/** Abstract grid with no data, just size
 *  Not really useful by itself, but a good ancestor to images, placed grid and everything
 */
class AbstractGrid {
protected:
    int size_w_;
    int size_h_;
public:
    explicit AbstractGrid(int width, int height);
    explicit AbstractGrid(pair<int> size);

    [[nodiscard]] inline int width() const { return this->size_w_; }
    [[nodiscard]] inline int height() const { return this->size_h_; }
    [[nodiscard]] inline pair<int> size() const { return {this->size_w_, this->size_h_}; }
};


#endif //ANASTASIS_CPP_ABSTRACTGRID_H
