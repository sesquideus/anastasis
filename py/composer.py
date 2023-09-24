

import numpy as np
import scipy as sp
import matplotlib as mpl

from typing import Optional, Iterable
from grid import Grid

np.set_printoptions(threshold=100000)


class MatrixComposer:
    def __init__(self,
                 data_grids: Iterable[Grid],
                 model: Grid,
                 ):
        self._data_grids = list(data_grids)
        self._model = model
        self._matrix = np.concatenate([grid.onto(self.model) for grid in self.data_grids], axis=0)
        self._vector = np.concatenate([grid._data.ravel() for grid in self.data_grids], axis=0)

    @property
    def data_grids(self):
        return self._data_grids

    @property
    def model(self):
        return self._model

    @property
    def matrix(self):
        return self._matrix

    @property
    def vector(self):
        return self._vector

    def create(self):
        for grid in self.data_grids:
            matrix = self.model.onto(grid)
            grid.data = matrix @ grid._data.flatten()

    def solution_matrix(self, covariance=None):
        if covariance is None:
            inv_covariance = np.identity(self.matrix.shape[0])
        else:
            inv_covariance = np.linalg.inv(covariance)
        assert inv_covariance.shape == (self.vector.size, self.vector.size)

        ATCinv = np.matmul(self.matrix.T, inv_covariance)
        assert ATCinv.shape == (self.model.size, self.vector.size)
        print(np.matmul(np.linalg.inv(np.matmul(ATCinv, self.matrix)), ATCinv).shape)
        #print(np.matmul(ATCinv, A))
        return np.matmul(np.linalg.inv(np.matmul(ATCinv, self.matrix)), ATCinv)

    def solve(self):
        return (self.solution_matrix() @ self.vector).reshape(self.model.shape)