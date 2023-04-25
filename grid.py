import numpy as np

from typing import Optional, Self


class Grid:
    def __init__(self,
                 xlim: tuple[float, float],
                 ylim: tuple[float, float],
                 rotation: float = 0,
                 *,
                 shape: tuple[int, int] = None,
                 data: Optional[np.ndarray] = None):
        self._xmin, self._xmax = xlim
        self._ymin, self._ymax = ylim
        self.rotation = rotation

        if data is None:
            assert shape is not None, "You must provide either shape or explicit data"
            self._width, self._height = shape
            self._data = np.zeros(shape=shape)
        else:
            assert shape is None, "You cannot provide explicit shape if data are provided"
            assert data.ndim == 2, "Data must be None or a 2D array"
            self._data = data
            self._width, self._height = data.shape

    @staticmethod
    def from_centre(centre: tuple[float, float],
                    size: tuple[float, float],
                    rotation: float = 0,
                    *,
                    shape: tuple[int, int] = None,
                    data: Optional[np.ndarray] = None):
        cx, cy = centre
        sx, sy = size
        return Grid((cx - sx / 2, cx + sx / 2),
                    (cy - sy / 2, cy + sy / 2),
                    rotation, shape=shape, data=data)

    @staticmethod
    def pixel_coords(xlim: tuple[float, float],
                     ylim: tuple[float, float],
                     count: tuple[int, int]):
        xmin, xmax = xlim
        ymin, ymax = ylim
        xcount, ycount = count
        xsize, ysize = (xmax - xmin) / xcount, (ymax - ymin) / ycount

        xs = np.linspace(xmin, xmax, xcount, endpoint=False) + xsize / 2
        ys = np.linspace(ymin, ymax, ycount, endpoint=False) + ysize / 2
        xgrid, ygrid = np.meshgrid(xs, ys)
        return np.stack((xgrid, ygrid), axis=2)

    @staticmethod
    def rot_matrix(angle: float):
        return np.array([
            [np.cos(angle), -np.sin(angle)],
            [np.sin(angle), np.cos(angle)]
        ])

    @property
    def width(self):
        return self._width

    @property
    def height(self):
        return self._height

    @property
    def shape(self):
        return (self._width, self._height)

    @property
    def size(self):
        return self._width * self._height

    @property
    def pixel_width(self):
        return (self._xmax - self._xmin) / self.width

    @property
    def pixel_height(self):
        return (self._ymax - self._ymin) / self.height

    def grid_pixels(self):
        return self.pixel_coords((self._xmin, self._xmax), (self._ymin, self._ymax), (self.width, self.height))

    def world_coords(self):
        mat = np.expand_dims(self.rot_matrix(self.rotation), (0, 1))
        pix = self.grid_pixels()
        return np.squeeze(mat @ np.expand_dims(pix, 3))

    def __str__(self):
        return f"Grid {self.width}×{self.height}, {self._xmin} to {self._xmax}"

    def __matmul__(self, other: Self) -> np.ndarray[float]:
        """
        Project this grid onto another grid and return a 4D overlap matrix
        """
        return None