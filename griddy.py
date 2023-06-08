#!/usr/env/bin python

import math
import numpy as np
import grid

from matplotlib import pyplot as plt
import matplotlib as mpl

np.set_printoptions(threshold=1e6)

big = grid.Grid((1, 2), (3, 5), rotation=math.tau / 8, shape=(3, 2))

#unit = grid.Grid.from_centre((0, 0), (1, 1), rotation=math.tau, shape=(1, 1))
#unit.print_grid()
#unit.print_world()
#rotated = grid.Grid.from_centre((0, 0), (1, 1), rotation=math.tau / 2, shape=(1, 1))
#rotated.print_grid()
#rotated.print_world()

### This is apparently wrong
model = grid.Grid((0, 0), (50, 50), rotation=0.02, shape=(40, 40))
data = grid.Grid((0, 0), (19, 11), rotation=0.03, shape=(20, 11))

#model = grid.Grid((0, 0), (50, 50), rotation=0.02, shape=(10, 10))
#data = grid.Grid((0, 0), (19, 11), rotation=0.53, shape=(10, 5))
#data = grid.Grid((1.1, 1.6), (2, 3), rotation=0, shape=(3, 2))
#model = grid.Grid((1.5, 2), (3, 4), rotation=0, shape=(4, 3))

overlap = data @ model
print(data.world_vertices[10, 5])
print(model.world_vertices[20, 19])
print(overlap[10, 5, 20, 19])
#
#
#
#for x in range(0, 20):
#    for y in range(0, 11):
overlap = data @ model
plt.imshow(np.sum(overlap, axis=(0, 1)),
           extent=(model.left, model.right, model.bottom, model.top),
           cmap='hot', vmin=0)
plt.colorbar()
plt.savefig(f'overlap.png')
plt.close('all')