import pytest
import numpy as np

from grid import Grid
from composer import MatrixComposer


@pytest.fixture
def composer():
    return MatrixComposer(
        [Grid((shift, 0), (7, 7), data=np.arange(49).reshape(7, 7)) for shift in np.linspace(0, 1, 3, endpoint=False)],
        Grid((0, 0), (10, 10), shape=(10, 10))
    )


class TestComposer:
    def test_positions(self):
        assert True

    def test_matrix_shape(self, composer):
        assert composer.matrix.shape == (147, 100)

    def test_matrix_value(self, composer):
        """
        For a full overlap each data pixel should be completely within the model grid
        """
        assert np.allclose(composer.matrix.sum(axis=1), np.ones(shape=147,))

    def test_solver(self, composer):
        print(composer.matrix)