import math
import pytest
import numpy as np

import grid
from grid import Grid


@pytest.fixture
def unit():
    return Grid.from_centre((0, 0), (2, 2), 0, shape=(3, 3))


@pytest.fixture
def large():
    return Grid.from_centre((0, 0), (20, 10), 35, shape=(10, 5))


@pytest.fixture
def rot90():
    return Grid.from_centre((0, 0), (2, 2), math.tau / 4, shape=(3, 3))


@pytest.fixture
def straight_unit():
    return Grid.from_centre((0, 0), (1, 1), 0, shape=(1, 1))


@pytest.fixture
def diagonal_unit():
    return Grid.from_centre((0, 0), (1, 1), math.tau / 8, shape=(1, 1))


class TestShape:
    def test_grid_centres_shape(self, unit):
        assert unit.grid_centres().shape == (3, 3, 2)

    def test_grid_vertices_shape(self, unit):
        assert unit.grid_vertices().shape == (3, 3, 2, 2, 2)

    def test_grid_centres(self, unit):
        assert np.allclose(unit.grid_centres()[0][0], (-2 / 3, -2 / 3))

    def test_grid_vertices_bottom_left(self, unit):
        assert np.allclose(unit.grid_vertices(pixfrac=1)[1][1][0][0], (-1 / 3, -1 / 3))

    def test_grid_vertices_bottom_right(self, unit):
        assert np.allclose(unit.grid_vertices(pixfrac=1)[1][1][0][1], (1 / 3, -1 / 3))

    def test_grid_vertices_top_left(self, unit):
        assert np.allclose(unit.grid_vertices(pixfrac=1)[1][1][1][0], (-1 / 3, 1 / 3))

    def test_grid_vertices_top_right(self, unit):
        assert np.allclose(unit.grid_vertices(pixfrac=1)[1][1][1][1], (1 / 3, 1 / 3))

    def test_grid_vertices_extra(self, unit):
        assert np.allclose(unit.grid_vertices(pixfrac=0.1)[0][0][0][0], (-21 / 30, -21 / 30))


class TestProperties:
    def test_size(self, large):
        assert large.shape == (10, 5)

    def test_pixel_size(self, large):
        assert np.allclose(large.pixel_size, (2, 2))

    def test_limits(self, large):
        assert large.width, large.height == (20, 10)

    def test_grid_centres(self, large):
        assert np.allclose(large.grid_centres()[3][7], (5, 2))


class TestCreation:
    def test_data_provided(self):
        grid = Grid.from_centre((0, 0), (5, 3), 0, data=np.zeros((3, 3)))
        assert grid.shape == (3, 3)

    def test_shape_provided(self):
        grid = Grid.from_centre((0, 0), (5, 3), 0, data=np.ones((3, 3)))
        assert np.allclose(grid._data, np.array([[1, 1, 1], [1, 1, 1], [1, 1, 1]]))

    def test_both_provided_correct(self):
        assert Grid.from_centre((0, 0), (5, 3), 0, shape=(3, 3), data=np.zeros((3, 3)))

    def test_both_provided_incorrect(self):
        with pytest.raises(AssertionError):
            _ = Grid.from_centre((0, 0), (5, 3), 0, shape=(3, 3), data=np.zeros((4, 5)))

    def test_none_provided(self):
        with pytest.raises(AssertionError):
            _ = Grid.from_centre((0, 0), (3, 8), math.tau / 2)


class TestRotation:
    def test_top_left(self, unit):
        c = np.cos(math.tau)
        s = np.sin(math.tau)
        assert np.allclose(unit.world_centres(), unit.grid_centres())

    def test_rot90(self, unit, rot90):
        assert np.allclose(rot90.world_centres(), np.rot90(unit.world_centres()))

    def test_rot90_static(self, unit, rot90):
        assert np.allclose(rot90.grid_centres(), unit.world_centres())


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
        assert np.allclose(
            inter,
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

    def test_shape(self, unit, large):
        assert (unit @ large).shape == (10, 5, 3, 3)

    def test_octagon(self, straight_unit, diagonal_unit):
        assert np.allclose(
            grid.intersect_rectangles(
                straight_unit.world_vertices()[0, 0], diagonal_unit.world_vertices()[0, 0]
            ), np.sqrt(8) - 2)

    def test_identity(self, straight_unit):
        assert np.allclose(
            grid.intersect_rectangles(
                straight_unit.world_vertices()[0, 0], straight_unit.world_vertices()[0, 0]
            ), 1)