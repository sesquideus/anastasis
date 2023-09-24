#!/usr/bin/env python

import math
import numpy as np
from matplotlib import pyplot as plt
import matplotlib as mpl

from grid import Grid
from composer import MatrixComposer

composer = MatrixComposer(
    [
        Grid((shift, 0), (50, 50), rotation=0, data=np.random.uniform(0, 1, size=(15, 15)))
        for shift in np.linspace(-0.5, 0.5, 3, endpoint=False)
    ],
    Grid((5, 10), (20, 20), shape=(20, 20))
)
composer.create()

def empty():
    fig, ax = plt.subplots(1, 1, figsize=(8, 5), dpi=300)
    fig.tight_layout()
    return fig, ax

def plot_original():
    fig, ax = empty()
    img = ax.imshow(original._data, cmap='hot', vmin=0, extent=original.extent, aspect=1)
    ax.set_aspect('equal')
    fig.colorbar(img)
    fig.savefig('output/original.png')

def plot_resampled():
    fig, ax = empty()
    img = ax.imshow(view._data, cmap='hot', vmin=0, extent=view.extent, aspect=1, interpolation='nearest')
    ax.set_aspect('equal')
    fig.colorbar(img)
    fig.savefig('output/resampled.png')


def plot_model():
    fig, ax = empty()
    print(composer.model._data)
    img = ax.imshow(composer.data_grids[0]._data, cmap='hot', vmin=0, origin='lower')
    fig.colorbar(img)
    fig.show()

def plot_matrix():
    fig, ax = empty()
    img = ax.imshow(composer.matrix)
    fig.suptitle("Matrix")
    fig.show()

def plot_contributions():
    for data in composer.data_grids:
        fig, ax = empty()
        fig.suptitle(data)
        img = ax.imshow((data @ composer.model).sum(axis=(2, 3)), extent=data.extent, vmin=0, origin='lower')
        fig.colorbar(img)
        fig.show()

def plot_solution():
    fig, ax = empty()
    img = ax.imshow(composer.solve(), cmap='hot', vmin=0, vmax=9, origin='lower')
    fig.colorbar(img)
    fig.show()


#plot_model()
#plot_matrix()
#plot_contributions()
#plot_solution()

original = Grid.load('pingvys.bmp')
view = Grid((0, 0), (60, 80), shape=(241, 181), rotation=np.radians(0))

print("Resampling...")
original.resample_onto(view)
print("Plotting the original")
plot_original()
print("Plotting resampled")
plot_resampled()

