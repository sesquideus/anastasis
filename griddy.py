#!/usr/bin/env python

import math
import numpy as np
import grid

from matplotlib import pyplot as plt
import matplotlib as mpl
from matplotlib import ticker

np.set_printoptions(threshold=1e6)
plt.rcParams["font.family"] = "Minion Pro"
plt.rcParams["font.size"] = 24

big = grid.Grid((1, 2), (3, 5), rotation=math.tau / 8, shape=(3, 2))

#unit = grid.Grid.from_centre((0, 0), (1, 1), rotation=math.tau, shape=(1, 1))
#unit.print_grid()
#unit.print_world()
#rotated = grid.Grid.from_centre((0, 0), (1, 1), rotation=math.tau / 2, shape=(1, 1))
#rotated.print_grid()
#rotated.print_world()

### This is apparently wrong
#model = grid.Grid((0, 0), (50, 50), rotation=0.02, shape=(15, 15))
#data = grid.Grid((0, 0), (29, 31), rotation=0.3, shape=(15, 15))
#
##model = grid.Grid((0, 0), (50, 50), rotation=0.02, shape=(10, 10))
##data = grid.Grid((0, 0), (19, 11), rotation=0.53, shape=(10, 5))
data = grid.Grid((0, 0), (20, 20), rotation=0, shape=(30, 20))
model = grid.Grid((0, 0), (17, 10), rotation=0.1, shape=(40, 30))
#
overlap = data @ model
#
def plot(filename, what, extent=None):
    fig, ax = plt.subplots(1, 1, figsize=(16, 10), dpi=200)
    fig.tight_layout()
    #ax.xaxis.set_minor_locator(ticker.MultipleLocator(base=1.25))
    #ax.yaxis.set_minor_locator(ticker.MultipleLocator(base=1.25))
    #ax.grid(which='both', axis='both', linestyle=':')
    img = ax.imshow(what, extent=extent, cmap='hot', vmin=0, origin='lower')
    fig.colorbar(img)
    fig.savefig(filename)
    plt.close('all')

plot('overlap.png', np.sum(overlap, axis=(0, 1)), extent=(model.left, model.right, model.bottom, model.top))
plot('contribution.png', np.sum(overlap, axis=(2, 3)), extent=(data.left, data.right, data.bottom, data.top))
plot('matrix.png', np.reshape(overlap, (overlap.shape[0] * overlap.shape[1], -1)))

