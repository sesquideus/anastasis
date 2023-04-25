import numpy as np

from typing import Optional

np.set_printoptions(threshold=100)


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
    def overlap(delta: np.ndarray[float],   # difference in positions along the axis
                data_pix_size: float,       # pixel size in the data grid
                model_pix_size: float       # pixel size in the model grid
                ) -> np.ndarray[float]:     # overlap of the pixels [area]
        return np.where(
            np.abs(delta) <= np.abs(data_pix_size - model_pix_size) / 2,
            1,
            np.where(
                np.abs(delta) >= (data_pix_size + model_pix_size) / 2,
                0,
                ((data_pix_size + model_pix_size) - 2 * np.abs(delta)) / (2 * np.minimum(model_pix_size, data_pix_size)),
            )
        )


    def single_matrix(self, data, model):
        dist = np.expand_dims(data.grid_pixels(), (0, 1)) - np.expand_dims(model.grid_pixels(), (2, 3))
        xover = self.overlap(dist[..., 0], data.pixel_width, model.pixel_width)
        yover = self.overlap(dist[..., 1], data.pixel_height, model.pixel_height)
        return xover * yover

    def multi_matrix(self, matrices, model):
        return np.stack(
            [self.single_matrix(matrix, model) for matrix in matrices],
            axis=4
        )

    def stacked_matrix(self, generator, model):
        return np.concatenate(
            [self.single_matrix(matrix, model).reshape(model.size, matrix.size) for matrix in generator],
            axis=0,
        )

    def solution(self):
        Cinv = np.linalg.inv(C)
        ATCinv = A.T @ Cinv
        return np.linalg.inv(ATCinv @ A) @ ATCinv


