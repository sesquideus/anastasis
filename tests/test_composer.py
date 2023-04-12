import pytest

from composer import MatrixComposer


@pytest.fixture
def composer():
    return MatrixComposer((0, 0), (0, 0))


class TestComposer:
    def test_positions(self):
        assert True

    def test_overlap(self, composer):
        assert composer.overlap(0.3, 0.5, 0.8) == 1
