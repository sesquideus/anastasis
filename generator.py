#!/usr/bin/env python

import numpy as np
import scipy as sp
import operator
import matplotlib as mpl

from PIL import Image
from matplotlib import pyplot as plt

from typing import Self

mpl.use('agg')


class Generator:
    def __init__(self, source):
        self._data = np.array(Image.open(source))
        self._data = self.bin(32, 32)

    def load(self, data: np.ndarray[float]):
        self._data = data

    @property
    def data(self):
        return self._data

    def bin(self, *bins: int) -> np.ndarray[float]:
        """ Produce a binning of a data cuboid. In each dimension the size must be divisible by the bin width. """
        shape = self._data.shape
        assert len(shape) == len(bins), f"Dimensions do not match: data are {shape}, binning by {bins}"
        assert sum(map(operator.mod, shape, bins)) == 0,\
            f"Some dimensions are not multiples of corresponding binning width: data are {shape}, binning by {bins}"

        # Each new dimension will be d_size // d_bin
        new_dim = tuple(map(operator.floordiv, shape, bins))
        # Interleave the dimensions (x, y, z) by (a, b, c) becomes (x // a, a, y // b, b, z // c, c)
        summing_dim = tuple(sum(zip(new_dim, bins), ()))
        # Sum the data in every odd bin
        return self._data.reshape(summing_dim).sum(axis=tuple(np.arange(1, 2 * len(shape), 2)))

    def pad(self, top, height):
        return np.pad(self._data, ((top, height - top), (0, 0)))

    def shift_bin(self, top, height):
        self._data = self.pad(top, height)
        self._data = self.bin(4, 1)
        return np.repeat(self._data, height, axis=0)


out = Generator('pingvys.bmp')
plt.imshow(out.data)
plt.savefig('out.pdf')

plt.imshow(sp.signal.convolve(out.data, np.ones(shape=(5, 1)), mode='same'))
plt.savefig('convolved.pdf')

out.bin(4, 1)
plt.imshow(out.data)
plt.savefig('binned.pdf')

for i in range(0, 4):
    out._data = out.shift_bin(i, 4)
    plt.imshow(out._data)
    plt.savefig(f'{i:02}.pdf')
