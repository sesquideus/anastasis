#!/usr/bin/env python

import numpy as np
import scipy as sp
import operator
import matplotlib as mpl

from PIL import Image
from matplotlib import pyplot as plt

from typing import Self

import imaging
import composer

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

    def shift_bin_grid(self,
                       offsets: tuple[int, ...],
                       steps: tuple[int, ...]
                      ) -> np.ndarray[float]:
        return np.repeat(self._data, height, axis=0)


class Generator:
    def __init__(self, imager):
        self.imager = imager

    def __call__(self, prefix):
        i = 0
        for image in imaging.shift_bin_grid(self.imager.data, (32, 32), (4, 4)):
            fig, ax = plt.subplots(figsize=(8, 12))
            ax.imshow(image, extent=[0, 128, 0, 192])
            fig.savefig(f'{prefix} {i:02d}.png')
            i += 1


imager = Imager('pingvys.bmp')
generator = Generator(imager)
generator('out/A')

comp = composer.MatrixComposer((0, 0), (0, 0))

data = composer.Grid((0, 10), (0, 10), (20, 20))
model = composer.Grid((0, 10), (0, 10), (10, 10))
multidata = [composer.Grid((-1 + i / 3, 10 + i / 3), (0, 10), (2, 10)) for i in range(0, 3)] +\
            [composer.Grid((0, 10), (-1 + i / 3, 10 + i / 3), (10, 2)) for i in range(0, 3)]

mat = comp.single_matrix(data, model)
multimat = comp.multi_matrix(multidata, model)

fig, ax = plt.subplots(1, 1, figsize=(8, 6))
ax.imshow(mat, cmap='hot')
fig.savefig('out/mat.png', dpi=300)

fig, ax = plt.subplots(1, 1, figsize=(8, 6))
ax.imshow(multimat, cmap='hot')
fig.savefig('out/multimat.png', dpi=300)
