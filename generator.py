#!/usr/bin/env python

import numpy as np
import scipy as sp
import operator
import matplotlib as mpl

from PIL import Image
from matplotlib import pyplot as plt

from typing import Self

import imaging

mpl.use('agg')


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
            padding: tuple[tuple[int, int]]):
        return imaging.bin_padded(self.data, bins, padding)

    def pad(self, top, height):
        return np.pad(self._data, ((top, height - top), (0, 0)))

    def shift_bin_grid(self, offsets: tuple[int, ...], steps: tuple[int, ...]):
        return np.repeat(self._data, height, axis=0)


class MatrixComposer:
    def __init__(self,
                 pixels: tuple[int, int],
                 pixsizes: tuple[int, int],
                 rotation: float = None,
                 ):
        xpixels, ypixels = pixels
        xsize, ysize = sizes

        xs = np.linspace(0, xpixels, xsize, endpoint=False)
        ys = np.linspace(0, ypixels, ysize, endpoint=False)

        xgrid, ygrid = np.meshgrid(xs, ys)

    def solution(self):
        Cinv = np.linalg.inv(C)
        ATCinv = A.T @ Cinv
        return np.linalg.inv(ATCinv @ A) @ ATCinv



imager = Imager('pingvys.bmp')
i = 0
for image in imaging.shift_bin_grid(imager.data, (32, 8), (16, 1)):
    fig, ax = plt.subplots(figsize=(8, 12))
    ax.imshow(image, extent=[0, 128, 0, 192])
    fig.savefig(f'{i:02d}.png')
    i += 1
