#!/usr/bin/env python

import numpy as np
import matplotlib as mpl

from PIL import Image
from matplotlib import pyplot as plt
from typing import Iterable

import imaging
import composer

from grid import Grid

mpl.use('agg')
np.set_printoptions(threshold=1000)


class Imager:
    def __init__(self, source):
        self._data = np.array(Image.open(source))

    def load(self, data: np.ndarray[float]):
        self._data = data

    @property
    def data(self):
        return self._data

    def bin(self,
            bins: tuple[int, ...]):
        return imaging.bin(self.data, bins)

    def bin_padded(self,
                   bins: tuple[int, ...],
                   padding: tuple[tuple[int, int], ...]):
        return imaging.bin_padded(self.data, bins, padding)

    def pad(self, top, height):
        return np.pad(self._data, ((top, height - top), (0, 0)))


class Generator:
    def __init__(self, imager):
        self.imager = imager

    def __call__(self, prefix):
        i = 0
        for image in imaging.shift_bin_grid(self.imager.data, (32, 32), (4, 4)):
            fig, ax = plt.subplots(figsize=(8, 12))
            ax.imshow(image, extent=[0, 128, 0, 192])
            fig.savefig(f'{prefix}{i:02d}.png')
            i += 1


def render_slices(matrix, datasize, modelsize):
    modelxsize, modelysize = modelsize
    assert matrix.ndim == 4
    print(matrix.shape)

    for i in range(0, modelysize):
        for j in range(0, modelxsize):
            fig, ax = plt.subplots(1, 1, figsize=(8, 6))
            ax.imshow(matrix[i, j, ...], extent=[0, 10, 0, 10], cmap='hot', vmin=0, vmax=1, alpha=1)
            ax.imshow(np.indices(modelsize).sum(axis=0) % 2, extent=[0, 10, 0, 10], cmap='gray', alpha=0.1)
            fig.savefig(f'out/mat{i:03d}-{j:03d}.png', dpi=300)
            plt.close('all')
            print(f"Matrix plotted for {i} {j}")

def subshift_and_rotate(xlim: tuple[int, int],
                        ylim: tuple[int, int],
                        count: tuple[int, int],
                        shifts: tuple[int, int],
                        rotations: Iterable[float] = (0,)):
    xmin, xmax = xlim
    ymin, ymax = ylim
    xcount, ycount = count
    xshift, yshift = shifts
    for rotation in rotations:
        for ys in ((np.arange(0, 2 * yshift, 2) - yshift + 1) / (2 * yshift)):
            for xs in ((np.arange(0, 2 * xshift, 2) - xshift + 1) / (2 * xshift)):
                if rotation == 0:
                    print((xmin + xs, xmax + xs), (ymin + ys, ymax + ys), (xcount, ycount))
                    yield Grid((xmin + xs, xmax + xs), (ymin + ys, ymax + ys), shape=(xcount, ycount))
                elif rotation == 90:
                    yield Grid((ymin + ys, ymax + ys), (xmin + xs, xmax + xs), shape=(ycount, xcount))
                else:
                    raise NotImplementedError("Rotation must be either 0 or 90 degrees")


def main():
    imager = Imager('pingvys.bmp')
    #generator = Generator(imager)
    #generator('out/A')

    comp = composer.MatrixComposer((0, 0), (0, 0))

    DATASIZE = (7, 12)
    MODELSIZE = (5, 5)
    data = Grid((0, 10), (0, 10), shape=DATASIZE)
    model = Grid((0, 10), (0, 10), shape=MODELSIZE)
    multidata = list(subshift_and_rotate((-3, 13), (3, 7), (16, 4), (3, 3), [0]))

    mat = comp.single_matrix(data, model)
    mat2d = mat.reshape(data.size, model.size)
    multimatrix = comp.stacked_matrix(multidata, model)

    #render_slices(mat, DATASIZE, MODELSIZE)

    fig, ax = plt.subplots(1, 1, figsize=(8, 6))
    print(multimatrix.shape)
    ax.imshow(multimatrix, cmap='hot')
    fig.savefig('out/multimat.png', dpi=300)

main()