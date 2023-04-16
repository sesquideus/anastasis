import numpy as np

from typing import Optional

np.set_printoptions(threshold=100)


class Grid:
    def __init__(self,
                 xlim: tuple[float, float],
                 ylim: tuple[float, float],
                 count: tuple[int, int],
                 data: Optional[np.ndarray] = None):
        self._data = data
        self._xmin, self._xmax = xlim
        self._ymin, self._ymax = ylim
        self._width, self._height = count

    @staticmethod
    def pixel_coords(xlim: tuple[float, float], ylim: tuple[float, float], count: tuple[int, int]):
        xmin, xmax = xlim
        ymin, ymax = ylim
        xcount, ycount = count
        xsize, ysize = (xmax - xmin) / xcount, (ymax - ymin) / ycount

        xs = np.linspace(xmin, xmax, xcount, endpoint=False) + xsize / 2
        ys = np.linspace(ymin, ymax, ycount, endpoint=False) + ysize / 2
        xgrid, ygrid = np.meshgrid(xs, ys)
        return np.stack((xgrid, ygrid), axis=2)

    @property
    def width(self):
        return self._width

    @property
    def height(self):
        return self._height

    @property
    def size(self):
        return self._width * self._height

    @property
    def pixel_width(self):
        return (self._xmax - self._xmin) / self.width

    @property
    def pixel_height(self):
        return (self._ymax - self._ymin) / self.height

    def pixels(self):
        return self.pixel_coords((self._xmin, self._xmax), (self._ymin, self._ymax), (self.width, self.height))


class MatrixComposer:
    def __init__(self,
                 pixels: tuple[int, int],
                 pixsizes: tuple[int, int],
                 rotation: float = None,
                 ):
        xpixels, ypixels = pixels
        xsize, ysize = pixsizes

        xs = np.linspace(0, xpixels, xsize, endpoint=False)
        ys = np.linspace(0, ypixels, ysize, endpoint=False)

        xgrid, ygrid = np.meshgrid(xs, ys)


    @staticmethod
    def overlap(delta: np.ndarray[float],
                data_pix_size: float,
                model_pix_size: float
                ) -> np.ndarray[float]:
        """
        Parameters
        ----------
            delta: float
                difference in positions along the axis
            data_pix_size: float
                pixel size in the data grid
            model_pix_size: float
                pixel size in the model grid

        Returns
        -------
            np.ndarray[float]: overlap of the pixels
        """
        return np.where(
            np.abs(delta) <= (data_pix_size - model_pix_size) / 2,
            1,
            np.where(
                np.abs(delta) >= (data_pix_size + model_pix_size) / 2,
                0,
                ((data_pix_size + model_pix_size) - 2 * np.abs(delta)) / (2 * model_pix_size),
            )
        )


    def single_matrix(self, data, model):
        dist = np.expand_dims(data.pixels(), (0, 1)) - np.expand_dims(model.pixels(), (2, 3))
        xover = self.overlap(dist[..., 0], data.pixel_width, model.pixel_width)
        yover = self.overlap(dist[..., 1], data.pixel_height, model.pixel_height)
        return xover * yover

    def multi_matrix(self, data, model):
        return np.concatenate([
            self.single_matrix(single, model).reshape(model.size, single.size) for single in data],
            axis=0
        )

    def solution(self):
        Cinv = np.linalg.inv(C)
        ATCinv = A.T @ Cinv
        return np.linalg.inv(ATCinv @ A) @ ATCinv


