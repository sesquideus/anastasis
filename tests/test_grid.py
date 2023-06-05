import math
import pytest
import numpy as np

import grid
from grid import Grid


@pytest.fixture
def unit():
    return Grid.from_centre((0, 0), (2, 2), rotation=0, shape=(3, 3))


@pytest.fixture
def small():
    return Grid.from_centre((0, 0), (2, 2), rotation=1, shape=(2, 5))


@pytest.fixture
def two_by_three():
    return Grid((0, 2), (0, 3), rotation=0, shape=(3, 2))


@pytest.fixture
def three_by_four():
    return Grid((0, 3), (0, 4), rotation=0, shape=(4, 3))


@pytest.fixture
def large():
    return Grid.from_centre((0, 0), (20, 10), rotation=35, shape=(10, 5))


@pytest.fixture
def rot90():
    return Grid.from_centre((0, 0), (2, 2), rotation=math.tau / 4, shape=(3, 3))


@pytest.fixture
def straight_unit():
    return Grid.from_centre((0, 0), (1, 1), rotation=0, shape=(1, 1))


@pytest.fixture
def diagonal_unit():
    return Grid.from_centre((0, 0), (1, 1), rotation=math.tau / 8, shape=(1, 1))


class TestShape:
    def test_grid_centres_shape(self, unit):
        assert unit.grid_centres().shape == (3, 3, 2)

    def test_grid_vertices_shape(self, unit):
        assert unit.grid_vertices().shape == (3, 3, 2, 2, 2)

    def test_grid_centres(self, unit):
        assert np.allclose(unit.grid_centres()[0][0], (-2 / 3, -2 / 3))

    def test_grid_vertices_bottom_left(self, unit):
        assert np.allclose(unit.grid_vertices()[1][1][0][0], (-1 / 3, -1 / 3))

    def test_grid_vertices_bottom_right(self, unit):
        assert np.allclose(unit.grid_vertices()[1][1][0][1], (1 / 3, -1 / 3))

    def test_grid_vertices_top_left(self, unit):
        assert np.allclose(unit.grid_vertices()[1][1][1][0], (-1 / 3, 1 / 3))

    def test_grid_vertices_top_right(self, unit):
        assert np.allclose(unit.grid_vertices()[1][1][1][1], (1 / 3, 1 / 3))

    def test_grid_vertices_extra(self, unit):
        unit._pixfrac = 0.1
        assert np.allclose(unit.grid_vertices()[0][0][0][0], (-21 / 30, -21 / 30))


class TestProperties:
    def test_size(self, large):
        assert large.shape == (10, 5)

    def test_pixel_size(self, large):
        assert np.allclose(large.pixel_size, (1, 4))

    def test_limits(self, large):
        assert large.width, large.height == (20, 10)

    def test_grid_centres(self, large):
        assert np.allclose(large.grid_centres()[7][3], (4, 2.5))


class TestCreation:
    def test_data_provided(self):
        grid = Grid.from_centre((0, 0), (5, 3), data=np.zeros((3, 3)))
        assert grid.shape == (3, 3)

    def test_shape_provided(self):
        grid = Grid.from_centre((0, 0), (5, 3), data=np.ones((3, 3)))
        assert np.allclose(grid._data, np.array([[1, 1, 1], [1, 1, 1], [1, 1, 1]]))

    def test_both_provided_correct(self):
        assert Grid.from_centre((0, 0), (5, 3), shape=(3, 3), data=np.zeros((3, 3)))

    def test_both_provided_incorrect(self):
        with pytest.raises(AssertionError):
            _ = Grid.from_centre((0, 0), (5, 3), shape=(3, 3), data=np.zeros((4, 5)))

    def test_none_provided(self):
        with pytest.raises(AssertionError):
            _ = Grid.from_centre((0, 0), (3, 8), rotation=math.tau / 2)


class TestRotation:
    def test_top_left(self, unit):
        c = np.cos(math.tau)
        s = np.sin(math.tau)
        assert np.allclose(unit.world_centres(), unit.grid_centres())

    def test_rot90(self, unit, rot90):
        assert np.allclose(rot90.world_centres(), np.rot90(unit.world_centres()))

    def test_rot90_static(self, unit, rot90):
        assert np.allclose(rot90.grid_centres(), unit.world_centres())


class TestLineSegment:
    def test_trivial(self):
        assert np.allclose(
            grid.segment_intersection(np.array([0, 0]),
                                      np.array([3, 6]),
                                      np.array([3, 0]),
                                      np.array([0, 6])),
            np.array([1.5, 3]))

    def test_generic(self):
        assert np.allclose(
            grid.segment_intersection(np.array([1, 1]),
                                      np.array([3, 6]),
                                      np.array([4, 2]),
                                      np.array([2, 7])),
            np.array([2.7, 5.25]))


class TestLineSegmentIntersection:
    def test_intersection(self):
        inter = grid.segment_intersection(
            np.array([[[0, 0], [1, 1], [-1, -1]], [[ 5,  7], [   3, 4],   [4, 2]]]),
            np.array([[[2, 0], [3, 6], [ 1,  1]], [[ 1, -3], [-9.5, 2.5], [4, 2]]]),
            np.array([[[0, 1], [4, 2], [ 1,  0]], [[-4, 10], [   7, 7],   [4, 2]]]),
            np.array([[[1, 0], [2, 7], [ 0,  1]], [[ 9, 12], [   5, 5],   [2, 7]]]),
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

    @pytest.mark.parametrize("p0,p1,q0,q1", [
        (np.array([0, 0]), np.array([7, 7]), np.array([7, 0]), np.array([8, 0])), # clearly non-intersecting
        (np.array([0, 0]), np.array([1, 0]), np.array([0, 1]), np.array([0, 2])), # perpendicular but out
        (np.array([0, 0]), np.array([1, 0]), np.array([1, 1]), np.array([0, 1])), # parallel segments
    ])
    def test_no_intersection(self, p0, p1, q0, q1):
        assert np.allclose(
            grid.segment_intersection(p0, p1, q0, q1),
            np.array([np.nan, np.nan]), equal_nan=True)


class TestOverlap:
    def test_shape(self, unit, small):
        assert (unit @ small).shape == (3, 3, 2, 5)

    def test_shape_inverse(self, small, unit):
        assert (small @ unit).shape == (2, 5, 3, 3)

    def test_shape_random(self):
        first = Grid.from_centre((0, 0), (1, 1), rotation=3.4, shape=(7, 8))
        second = Grid.from_centre((0, 0), (1, 1), rotation=math.tau / 2, shape=(9, 10))
        assert (first @ second).shape == (7, 8, 9, 10)

    @pytest.mark.parametrize("angle,expected", [
        (1e-6, 1 - 0.5e-6), # for tiny angles it is about 1 - 0.5 * angle
        (1e-5, 1 - 0.5e-5), # same
        (1e-4, 1 - 0.5e-4), # same
        (math.tau / 8, np.sqrt(8) - 2) # for 45° we get an octagon
    ])
    def test_squares(self, straight_unit, angle, expected):
        microrotated = Grid.from_centre((0, 0), (1, 1), rotation=angle, shape=(1, 1))
        assert np.allclose(
            grid.intersect_rectangles(
                straight_unit.world_vertices().reshape(-1, 2, 2, 2), microrotated.world_vertices().reshape(-1, 2, 2, 2)
            ), expected, rtol=1e-6)

    @pytest.mark.parametrize("r,c", [(4, 4), (7, 9), (10, 20)])
    @pytest.mark.parametrize("rot", [0, math.tau / 8, math.tau / 4])
    def test_overlap_identic(self, r, c, rot):
        """
        Overlap of a grid with itself should be a unit matrix
        """
        model = Grid((0, 1), (5, 6), rotation=rot, shape=(r, c))
        overlap = model._overlap_aligned(model).reshape(r * c, r * c)
        assert np.allclose(np.eye(r * c), overlap)

    @pytest.mark.parametrize("x,y,expected", [
        (0, 0, 1),      # total overlap
        (0, 1, 0.25),   # partial overlap, one side
        (1, 0, 0.25),   # partial overlap, other side
        (1, 1, 0.0625), # partial overlap, corner
        (3, 4, 0),      # no overlap with distant pixel
    ])
    def test_aligned(self, x, y, expected):
        """
        Test simple aligned grids -- this can be visualized easily
        """
        model = Grid((0, 10), (0, 10), shape=(8, 8))
        data = Grid((0, 10), (0, 10), shape=(10, 10))

        overlap = model._overlap_aligned(data)
        assert np.allclose(overlap[0, 0, x, y], expected)

    @pytest.mark.parametrize("x,y,expected", [
        (0, 0, 1),      # total overlap
        (0, 1, 0),      # no overlap with adjacent pixel
        (1, 0, 0),      # no overlap with adjacent pixel
        (1, 1, 0),      # no overlap with diagonally adjacent pixel
        (2, 2, 0),      # no overlap with distant pixel
    ])
    def test_overlapped(self, two_by_three, three_by_four, x, y, expected):
        overlap = two_by_three @ three_by_four
        print(overlap)
        assert np.allclose(overlap[0, 0, x, y], expected)

