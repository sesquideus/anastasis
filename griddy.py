#!/usr/env/bin python

import numpy as np
import grid


inter = grid.segment_intersection(
    np.array([[[0, 0], [1, 1], [-1, -1]], [[5, 7], [3, 4], [4, 2]]]),
    np.array([[[2, 0], [3, 6], [1, 1]], [[1, -3], [-9.5, 2.5], [4, 2]]]),
    np.array([[[0, 1], [4, 2], [1, 0]], [[-4, 10], [7, 7], [4, 2]]]),
    np.array([[[1, 0], [2, 7], [0, 1]], [[9, 12], [5, 5], [2, 7]]]),
)
