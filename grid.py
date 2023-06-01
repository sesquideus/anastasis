import math
import numpy as np

from typing import Optional, Self


def filter_nans(array, *, axis=0):
    return array[~np.isnan(array).any(axis=axis)]

def segment_intersection(p0, p1, q0, q1):
    p = p1 - p0
    q = q1 - q0
    b = q0 - p0
    det = np.cross(p, q)
    return np.where(
        np.expand_dims(det == 0, det.ndim),
        _segment_intersection_degenerate(p, q, b),
        p0 + _segment_intersection_nondegenerate(p, q, b) * p,
    )

def intersect_rectangles(r1: np.ndarray[float], r2: np.ndarray[float]) -> float:
    assert r1.shape[-3:] == (2, 2, 2), r1.shape
    assert r2.shape[-3:] == (2, 2, 2), r2.shape
    #print(r1)
    #print(r2)
    is1in2 = np.array([
        _is_inside_parallelogram(r1[0][0], r1[0][1], r1[1][0], r2[0][0]),
        _is_inside_parallelogram(r1[0][0], r1[0][1], r1[1][0], r2[0][1]),
        _is_inside_parallelogram(r1[0][0], r1[0][1], r1[1][0], r2[1][0]),
        _is_inside_parallelogram(r1[0][0], r1[0][1], r1[1][0], r2[1][1]),
    ])
    is2in1 = np.array([
        _is_inside_parallelogram(r2[0][0], r2[0][1], r2[1][0], r1[0][0]),
        _is_inside_parallelogram(r2[0][0], r2[0][1], r2[1][0], r1[0][1]),
        _is_inside_parallelogram(r2[0][0], r2[0][1], r2[1][0], r1[1][0]),
        _is_inside_parallelogram(r2[0][0], r2[0][1], r2[1][0], r1[1][1]),
    ])
    inter = np.array([
        _segment_intersection_general(r1[0][0], r1[0][1], r2[0][0], r2[0][1]),
        _segment_intersection_general(r1[0][1], r1[1][1], r2[0][0], r2[0][1]),
        _segment_intersection_general(r1[1][1], r1[1][0], r2[0][0], r2[0][1]),
        _segment_intersection_general(r1[1][0], r1[0][0], r2[0][0], r2[0][1]),
        _segment_intersection_general(r1[0][0], r1[0][1], r2[0][1], r2[1][1]),
        _segment_intersection_general(r1[0][1], r1[1][1], r2[0][1], r2[1][1]),
        _segment_intersection_general(r1[1][1], r1[1][0], r2[0][1], r2[1][1]),
        _segment_intersection_general(r1[1][0], r1[0][0], r2[0][1], r2[1][1]),
        _segment_intersection_general(r1[0][0], r1[0][1], r2[1][1], r2[1][0]),
        _segment_intersection_general(r1[0][1], r1[1][1], r2[1][1], r2[1][0]),
        _segment_intersection_general(r1[1][1], r1[1][0], r2[1][1], r2[1][0]),
        _segment_intersection_general(r1[1][0], r1[0][0], r2[1][1], r2[1][0]),
        _segment_intersection_general(r1[0][0], r1[0][1], r2[1][0], r2[0][0]),
        _segment_intersection_general(r1[0][1], r1[1][1], r2[1][0], r2[0][0]),
        _segment_intersection_general(r1[1][1], r1[1][0], r2[1][0], r2[0][0]),
        _segment_intersection_general(r1[1][0], r1[0][0], r2[1][0], r2[0][0]),
    ])

    # Add all 4 + 4 + 16 intersections points together, at most 8 of them are not nan
    vertices = np.concatenate((is1in2, is2in1, inter), axis=0)
    # Shift the frame to centre of mass, which is guaranteed to be inside the polygon
    centre = np.nansum(vertices, axis=0) / 24
    shifted = vertices - centre
    # Sort the vertices by azimuth from the centre
    azimuths = np.arctan2(shifted[:, 1], shifted[:, 0])
    shifted = shifted[azimuths.argsort()]
    # Replace all empty vertices with centre
    isok = np.any(~np.isnan(shifted), axis=1)
    first = np.tile(shifted[0, :], (24, 1)).T
    shifted = np.where(isok, shifted.T, first).T
    shoelace = 0.5 * np.abs(
        np.sum(shifted[:, 0] * np.roll(shifted[:, 1], -1)) -
        np.sum(shifted[:, 1] * np.roll(shifted[:, 0], -1))
    )
    return shoelace

def _is_inside_parallelogram(o, p, q, x):
    return _is_inside_parallelogram_origin(p - o, q - o, x - o)

def _is_inside_parallelogram_origin(p, q, x):
    #print("PQX", p, q, x)
    t = np.dot(p, x) / np.dot(p, p)
    u = np.dot(q, x) / np.dot(q, q)
    #print("TU", t, u)
    return np.where((0 <= t) & (t <= 1) & (0 <= u) & (u <= 1), x, np.full(shape=(2,), fill_value=np.nan))

def _segment_intersection_general(p0, p1, q0, q1):
    t = _segment_intersection_nondegenerate(p1 - p0, q1 - q0, q0 - p0)
    #print("Intersect", p0, p1, q0, q1, t)
    return np.where(np.isnan(t), np.full((2,), np.nan), p0 + t * (p1 - p0))


def _segment_intersection_degenerate(p, q, b):
    return np.full(p.shape, np.nan)


def _segment_intersection_nondegenerate(p, q, b):
    #print("PQB", p, q, b)
    det = np.cross(p, q)
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
        bottom_left     = centres + pixfrac * np.array((-xsize / 2, -ysize / 2))
        bottom_right    = centres + pixfrac * np.array(( xsize / 2, -ysize / 2))
        top_left        = centres + pixfrac * np.array((-xsize / 2,  ysize / 2))
        top_right       = centres + pixfrac * np.array(( xsize / 2,  ysize / 2))
        # Stack bottom and top together, keeping row-major order within pixels
        left = np.stack((bottom_left, top_left), axis=2)
        right = np.stack((bottom_right, top_right), axis=2)
        total = np.stack((left, right), axis=3)
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
        return np.squeeze(mat @ np.expand_dims(pix, 3), axis=3)

    def world_vertices(self, *, pixfrac: float = 1):
        mat = self.rot_matrix(self.rotation)
        pix = self.grid_vertices(pixfrac=pixfrac)
      #  print("Rotation matrix:", mat)
      #  print("Pixel matrix:", pix)
        a = mat @ np.swapaxes(pix, 3, 4)
        a = np.swapaxes(a, 3, 4)
      #  print("Rotated matrix:", a)
        return a

    def __str__(self):
        return f"Grid {self.width}×{self.height}, {self._xmin} to {self._xmax}, rotated by {self.rotation:.6f}"

    def _print(self, func):
        vertices = self.world_vertices(pixfrac=1)
        for row in range(self.height):
            for col in range(self.width):
                print(vertices)
                rect = vertices[row][col]
                for v in [1, 0]:
                    for h in [0, 1]:
                        print(f"{h} {v} {rect[v][h][0]:+.6f}, {rect[v][h][1]:+.6f}", end=' | ' if h == 0 else '')
                    print()

    def print_grid(self):
        return self._print(self.grid_vertices(pixfrac=1))

    def print_world(self):
        return self._print(self.world_vertices(pixfrac=1))

    def _intersect_trivial(self, other: Self) -> np.ndarray[float]:
        assert np.abs(np.fmod(self.rotation - other.rotation, math.tau)) < 1e-12,\
            f"{__name__} should not be used when rotations are not the same"

        return 0

    def _intersect_general(self, other: Self) -> np.ndarray[float]:
        assert np.abs(np.fmod(self.rotation - other.rotation, math.tau / 4)) >= 1e-12, \
            f"{__name__} should not be used when rotations are very similar to each other"

        result = intersect_rectangles(self.world_vertices(), other.world_vertices())

        assert result.shape == (self.height, self.width, other.height, other.width)
        return result

    def __matmul__(self, other: Self) -> np.ndarray[float]:
        """
        Project this grid onto another grid and return a 4D overlap matrix
        """
        if np.abs(np.fmod(self.rotation - other.rotation, math.tau / 4)) < 1e-12:
            return self._intersect_trivial(other)
        else:
            return self._intersect_general(other)
