#!/usr/env/bin python

import math
import numpy as np
import grid


#inter = grid.segment_intersection(
#    np.array([[[0, 0], [1, 1], [-1, -1]], [[5, 7], [3, 4], [4, 2]]]),
#    np.array([[[2, 0], [3, 6], [1, 1]], [[1, -3], [-9.5, 2.5], [4, 2]]]),
#    np.array([[[0, 1], [4, 2], [1, 0]], [[-4, 10], [7, 7], [4, 2]]]),
#    np.array([[[1, 0], [2, 7], [0, 1]], [[9, 12], [5, 5], [2, 7]]]),
#)


big = grid.Grid.from_centre((1, 2), (3, 5), math.tau / 8, shape=(3, 2))

unit = grid.Grid.from_centre((0, 0), (1, 1), 0, shape=(2, 2))
#unit.print_grid()
#unit.print_world()
rotated = grid.Grid.from_centre((0, 0), (1, 1), math.tau / 8, shape=(2, 2))
#rotated.print_grid()
#rotated.print_world()

print(grid.intersect_rectangles(
    unit.world_vertices()[0][0], rotated.world_vertices()[0][0]
))