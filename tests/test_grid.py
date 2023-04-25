import math
import pytest
import numpy as np

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
        assert np.allclose(unit.world_coords(), unit.grid_pixels())

    def test_rot90(self, unit, rot90):
        assert np.allclose(rot90.world_coords(), np.rot90(unit.world_coords()))

    def test_rot90_static(self, unit, rot90):
        assert np.allclose(rot90.grid_pixels(), unit.world_coords())
