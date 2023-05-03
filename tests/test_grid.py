import math
import pytest
import numpy as np

import grid
from grid import Grid


@pytest.fixture
def unit():
    return Grid.from_centre((0, 0), (2, 2), 0, shape=(3, 3))


@pytest.fixture
def rot90():
    return Grid.from_centre((0, 0), (2, 2), math.tau / 4, shape=(3, 3))


class TestRotation:
    def test_top_left(self, unit):
        c = np.cos(math.tau)
        s = np.sin(math.tau)
        assert np.allclose(unit.world_coords(), unit.grid_coords())

    def test_rot90(self, unit, rot90):
        assert np.allclose(rot90.world_coords(), np.rot90(unit.world_coords()))

    def test_rot90_static(self, unit, rot90):
        assert np.allclose(rot90.grid_coords(), unit.world_coords())



class TestIntersection:
    def test_single(self):
        assert np.allclose(
            grid.segment_intersection(np.array([1, 1]),
                                      np.array([3, 6]),
                                      np.array([4, 2]),
                                      np.array([2, 7])),
            np.array([2.7, 5.25]))

    def test_multiple(self):
        inter = grid.segment_intersection(
            np.array([[[0, 0], [1, 1], [-1, -1]], [[5, 7], [3, 4], [4, 2]]]),
            np.array([[[2, 0], [3, 6], [1, 1]], [[1, -3], [-9.5, 2.5], [4, 2]]]),
            np.array([[[0, 1], [4, 2], [1, 0]], [[-4, 10], [7, 7], [4, 2]]]),
            np.array([[[1, 0], [2, 7], [0, 1]], [[9, 12], [5, 5], [2, 7]]]),
        )
        assert np.allclose(inter,
            np.array([
                [
                    [1.0, 0.0],
                    [2.7, 5.25],
                    [0.5, 0.5],
                ],
                [
                    [np.nan, np.nan],
                    [np.nan, np.nan],
                    [np.nan, np.nan],
                ]
            ]),
            equal_nan=True,
        )

    def test_nan(self):
        assert np.allclose(
            grid.segment_intersection(np.array([0, 0]),
                                      np.array([7, 7]),
                                      np.array([7, 0]),
                                      np.array([8, 0])),
            np.array([np.nan, np.nan]), equal_nan=True)