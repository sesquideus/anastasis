import math
import numpy as np

from typing import Optional, Self


def segment_intersection(p0, p1, q0, q1):
    p = p1 - p0
    q = q1 - q0
    b = q0 - p0
    det = np.cross(p, q)
    return np.where(
        np.expand_dims(det == 0, det.ndim),
        _segment_intersection_degenerate(p, q, b),
        p0 + _segment_intersection_nondegenerate(p, q, b, det) * p,
    )


def _segment_intersection_general(p0, p1, q0, q1):
    t = _segment_intersection_nondegenerate(p1 - p0, q1 - q0, q0 - p0, 1)
    return np.where(np.isnan(t), np.full((2,), np.nan), p0 + t * (p1 - p0))


def _segment_intersection_degenerate(p, q, b):
    return np.full(p.shape, np.nan)


def _segment_intersection_nondegenerate(p, q, b, det):
    t = np.divide(np.cross(b, q), det, where=det != 0)
    u = np.divide(np.cross(b, p), det, where=det != 0)
    t = np.expand_dims(t, t.ndim)
    u = np.expand_dims(u, u.ndim)
    return np.where((0 <= t) & (t <= 1) & (0 <= u) & (u <= 1), t, np.nan)


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
            assert shape is not None, "You must provide either shape or explicit initial data"
            self._width, self._height = shape
            self._data = np.zeros(shape=shape)
        else:
            assert shape is None or shape == data.shape,\
                "Provided shape and initial data shape do not match (use shape=None to infer shape from data)"
            assert data.ndim == 2, "Data must form a 2D array"
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
    def overlap(centre_model: np.ndarray[float],        # world positions of centres of model pixels (M, N, 2)
                centre_data: np.ndarray[float],         # world positions of centres of data pixels  (P, Q, 2)
                size_model: tuple[float, float],        # width and height of model pixels
                size_data: tuple[float, float],         # width and height of data pixels
                rotation_model: float,                  # rotation of the model pixels
                rotation_data: float)\
                -> np.ndarray[float]:                   # (M, N, P, Q)
        #mnw = centre_model + rotate(0.5 * size_model * np.ndarray((1, 1)), rotation_model)
        #mne = centre_model + rotate(0.5 * size_model * np.ndarray((-1, 1)), rotation_model)
        #msw =
        #mse =

        pass

    @staticmethod
    def pixel_centres(xlim: tuple[float, float],
                      ylim: tuple[float, float],
                      count: tuple[int, int]):
        xmin, xmax = xlim
        ymin, ymax = ylim
        xcount, ycount = count
        xsize, ysize = (xmax - xmin) / xcount, (ymax - ymin) / ycount

        xs = np.linspace(xmin, xmax, xcount, endpoint=False) + xsize / 2
        ys = np.linspace(ymin, ymax, ycount, endpoint=False) + ysize / 2
        return np.stack(np.meshgrid(xs, ys), axis=2)

    @staticmethod
    def pixel_vertices(xlim: tuple[float, float],
                       ylim: tuple[float, float],
                       count: tuple[int, int],
                       *,
                       pixfrac: float = 1):
        """
        Returns a 5D array of pixel boundaries W×H×2×2×2, whose indices are
            vertical coordinate of the pixel
            horizontal coordinate of the pixel
            vertical component of each pixel rectangle (0: bottom, 1: top)
            horizontal component of each pixel rectangle (0: left, 1: right)
            actual coordinates (0: x, 1: y)
        so that R[5][7][0][1][1] is the y coordinate of the top-left corner of the pixel at (7, 5).
        Numpy uses row-major order, so we are keeping it
        """
        xmin, xmax = xlim
        ymin, ymax = ylim
        xcount, ycount = count
        xsize, ysize = (xmax - xmin) / xcount, (ymax - ymin) / ycount

        centres = Grid.pixel_centres(xlim, ylim, count)
        bottom_left     = centres + pixfrac * np.array((-xsize, -ysize))
        bottom_right    = centres + pixfrac * np.array(( xsize, -ysize))
        top_left        = centres + pixfrac * np.array((-xsize,  ysize))
        top_right       = centres + pixfrac * np.array(( xsize,  ysize))
        # Stack bottom and top together, keeping row-major order within pixels
        bottom = np.stack((bottom_left, bottom_right), axis=2)
        top    = np.stack((top_left, top_right), axis=2)
        total = np.stack((bottom, top), axis=3)
        return total

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
        return self._width, self._height

    @property
    def size(self):
        return self._width * self._height

    @property
    def pixel_width(self):
        return (self._xmax - self._xmin) / self.width

    @property
    def pixel_height(self):
        return (self._ymax - self._ymin) / self.height

    @property
    def pixel_size(self):
        return (self.pixel_width, self.pixel_height)

    def grid_centres(self):
        return self.pixel_centres(
            (self._xmin, self._xmax),
            (self._ymin, self._ymax),
            (self.width, self.height),
        )

    def grid_vertices(self, *, pixfrac: float = 1):
        return self.pixel_vertices(
            (self._xmin, self._xmax),
            (self._ymin, self._ymax),
            (self.width, self.height),
            pixfrac=pixfrac,
        )

    def world_centres(self):
        mat = np.expand_dims(self.rot_matrix(self.rotation), (0, 1))
        pix = self.grid_centres()
        return np.squeeze(mat @ np.expand_dims(pix, 3))

    def world_vertices(self, *, pixfrac: float = 1):
        mat = np.expand_dims(self.rot_matrix(self.rotation), (0, 1))
        pix = self.pixel_vertices(pixfrac=pixfrac)
        return np.squeeze(mat @ np.expand_dims(pix, 3))

    def __str__(self):
        return f"Grid {self.width}×{self.height}, {self._xmin} to {self._xmax}, rotated by {self.rotation:.6f}"

    def _intersect_trivial(self, other: Self) -> np.ndarray[float]:
        assert math.abs(math.fmod(self.rotation - other.rotation, math.tau)) < 1e-12,\
            f"{__name__} should not be used when rotations are not the same"

    def _intersect_general(self, other: Self) -> np.ndarray[float]:
        assert math.abs(math.fmod(self.rotation - other.rotation, math.tau / 4)) >= 1e-12, \
            f"{__name__} should not be used when rotations are very similar to each other"

    def __matmul__(self, other: Self) -> np.ndarray[float]:
        """
        Project this grid onto another grid and return a 4D overlap matrix
        """
        if math.abs(math.fmod(self.rotation - other.rotation, math.tau)) < 1e-15:
            return self._intersect_trivial(other)
        else:
            return self._intersect_general(other)
