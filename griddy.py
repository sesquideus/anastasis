#!/usr/env/bin python

import math
import numpy as np
import grid

from matplotlib import pyplot as plt
import matplotlib as mpl

np.set_printoptions(threshold=1e6)

big = grid.Grid.from_centre((1, 2), (3, 5), rotation=math.tau / 8, shape=(3, 2))

#unit = grid.Grid.from_centre((0, 0), (1, 1), rotation=math.tau, shape=(1, 1))
#unit.print_grid()
#unit.print_world()
#rotated = grid.Grid.from_centre((0, 0), (1, 1), rotation=math.tau / 2, shape=(1, 1))
#rotated.print_grid()
#rotated.print_world()

model = grid.Grid.from_centre((0, 0), (50, 50), rotation=0, shape=(40, 40))
data = grid.Grid.from_centre((0, 0), (29, 3), rotation=2, shape=(2, 11))
overlap = data @ model

print(np.sum(overlap, axis=(0, 1)))
plt.imshow(np.sum(overlap, axis=(0, 1)),
           extent=(model.left, model.right, model.bottom, model.top),
           cmap='hot', vmin=0)
plt.colorbar()
plt.savefig('out.pdf')