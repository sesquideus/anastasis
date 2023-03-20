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

    @staticmethod
    def pad_vertical(data: np.ndarray[float], top: int, height: int):
        return self.pad(data, )

    def shift_bin(self, offsets: tuple[int, ...], steps: tuple[int, ...]):
        return np.repeat(self._data, height, axis=0)


imager = Imager('pingvys.bmp')
i = 0
for image in imaging.shift_bin(imager.data, (128, 32), (4, 1)):
    plt.imshow(image, extent=[0, 32, 0, 48])
    print(image.shape)
    plt.savefig(f'{i:02}.png')
    i += 1
