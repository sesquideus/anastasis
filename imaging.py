import itertools
import operator
import numpy as np


def bin(data: np.ndarray[float],
        bins: tuple[int, ...]
       ) -> np.ndarray[float]:
    """
    Produce a binning of a data cuboid. In each dimension the size must be divisible by the bin width.
    """
    assert len(data.shape) == len(bins),\
        f"Dimensions do not match: data are {data.shape}, binning by {bins}"
    assert sum(map(operator.mod, data.shape, bins)) == 0,\
        f"Some dimensions are not multiples of corresponding binning width: data are {data.shape}, binning by {bins}"

    # Each new dimension will be d_size // d_bin
    new_dim = tuple(map(operator.floordiv, data.shape, bins))
    # Interleave the dimensions (x, y, z) by (a, b, c) becomes (x // a, a, y // b, b, z // c, c)
    summing_dim = tuple(sum(zip(new_dim, bins), ()))
    # Sum the data in every odd bin, reshaped automatically
    return data.reshape(summing_dim).sum(axis=tuple(np.arange(1, 2 * len(data.shape), 2)))


def bin_padded(data: np.ndarray[float],
               bins: tuple[int, ...],
               padding: tuple[tuple[int, int], ...]
              ) -> np.ndarray[float]:
    """
    Produce a binning but with sub-pixel offset
    """
    assert len(bins) == len(padding)
    data = np.pad(data, padding)
    return bin(data, bins)


def shift_bin(data: np.ndarray[float],
              sizes: tuple[int, ...],
              steps: tuple[int, ...]
             ) -> np.ndarray[float]:
    assert len(sizes) == len(steps),\
        f"Sizes and steps should be of equal length: sizes are {sizes}, steps are {steps}"
    for size, step in zip(sizes, steps):
        assert size % step == 0,\
            f"Each size must be divisible by the number of corresponding steps: offset is {offset}, step is {step}"

    ranges = itertools.product(*list([list(range(0, offset, offset // step)) for offset, step in zip(sizes, steps)]))

    for offset in ranges:
        print(sizes, offset)
        padding = tuple([(off, size - off) for size, off in zip(sizes, offset)])
        print(padding)
        yield bin_padded(data, sizes, padding)
