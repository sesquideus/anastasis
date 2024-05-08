#include "box.h"

namespace Astar {
    Pixel Box::as_pixel() const {
        return Pixel(*this);
    }

    real interval_overlap(pair<real> a, pair<real> b) {
        if ((a.second <= b.first) || (a.first >= b.second)) {
            return 0;
        } else {
            return std::min(a.second, b.second) - std::max(a.first, b.first);
        }
    }

    real Box::overlap(const Box & other) const {
        return interval_overlap({this->left, this->right}, {other.left, other.right}) *
               interval_overlap({this->bottom, this->top}, {other.bottom, other.top});
    }
}
