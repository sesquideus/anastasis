import math
import numpy as np

from typing import Optional, Self, Union


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

def intersect_rectangles(r1: np.ndarray[float], r2: np.ndarray[float]) -> np.ndarray[float]:
    """
    Takes two arrays of rectangles and computes the area of their overlap
    Works when none of the edges are aligned
    """
    assert r1.shape[-3:] == (2, 2, 2), f"r1 shape is {r1.shape}, expected (..., 2, 2, 2)"
    assert r2.shape[-3:] == (2, 2, 2), f"r2 shape is {r2.shape}, expected (..., 2, 2, 2)"

    # Find vertices of one rectangle that lie within the other, and vice versa
    inside = np.concatenate([
        np.array([
            _is_inside_parallelogram(r1[:, 0, 0, :], r1[:, 0, 1, :], r1[:, 1, 0, :], r2[:, (x & 2) >> 1, x & 1, :]),
            _is_inside_parallelogram(r2[:, 0, 0, :], r2[:, 0, 1, :], r2[:, 1, 0, :], r1[:, (x & 2) >> 1, x & 1, :]),
        ]) for x in range(0, 4)
    ], axis=0)
    # Find nontrivial intersections of all pairs of edges
    # The index cycles through the edges of both rectangles in a clever way
    # See https://ksvi.mff.cuni.cz/~kryl/dokumentace.htm#koment, last line:
    # "Definitely add comments to lines where you are proud of how clever your solution is.
    # From my own experience, it pays off to start looking for insidious bugs right there."
    intersections = np.array([
        _segment_intersection_general(r1[:, (      x & 2) >> 1, ((x + 1) & 2) >> 1, :],
                                      r1[:, ((x + 1) & 2) >> 1, ((x + 2) & 2) >> 1, :],
                                      r2[:, (      x & 8) >> 3, ((x + 4) & 8) >> 3, :],
                                      r2[:, ((x + 4) & 8) >> 3, ((x + 8) & 8) >> 3, :])
        for x in range(0, 16)
    ])

    # Add all 4 + 4 + 16 intersections points together, at most 8 of them are not nan
    vertices = np.concatenate((inside, intersections), axis=0)
    print(vertices)
    # Shift the frame to centre of mass, which is guaranteed to be inside the (convex) polygon
    centre = np.nansum(vertices, axis=0) / 24
    shifted = vertices - centre
    # Sort the vertices by azimuth from the centre of mass
    azimuths = np.arctan2(shifted[..., 1], shifted[..., 0])
    shifted = np.take_along_axis(shifted, azimuths.argsort(axis=0)[..., np.newaxis], 0)
    # Replace all empty vertices with the zeroth vertex
    isok = np.expand_dims(np.any(~np.isnan(shifted), axis=2), 2)
    zeroth = np.tile(shifted[0, :], (24, 1, 1))
    shifted = np.where(isok, shifted, zeroth)
    # It is enough to take first 8 points as there cannot be more intersections:
    # all further points are now guaranteed to be same as the zeroth
    #shifted = shifted[:8, ...]
    # Compute the quasideterminant: total area is given by the sum of areas of all triangles
    # (centre -- p_x -- p_(x+1))
    shoelace = 0.5 * np.abs(
        np.sum(shifted[..., 0] * np.roll(shifted[..., 1], -1, axis=0), axis=0) -
        np.sum(shifted[..., 1] * np.roll(shifted[..., 0], -1, axis=0), axis=0)
    )
    # Finally reshape to the shape of Cartesian product of the original inputs
    return shoelace.reshape((r1.shape[0], r2.shape[0]))

def _is_inside_parallelogram(o, p, q, x):
    """
    Determine whether point x is within the parallelogram determined by points o, p, q,
    that is (o, p, q, p+q-o) = o + (0, p, q, p+q)
    """
    o = np.expand_dims(o, 0)
    p = np.expand_dims(p, 0)
    q = np.expand_dims(q, 0)
    x = np.expand_dims(x, 1)
    return (o + _is_inside_parallelogram_origin(p - o, q - o, x - o)).reshape(-1, 2)

def _is_inside_parallelogram_origin(p, q, x):
    """
    Determine whether point x is within the oriented parallelogram determined by vectors p and q
    Returns x if True, np.ndarray([nan, nan]) if false
    """
    t = np.multiply(p, x).sum(axis=2) / np.multiply(p, p).sum(axis=2)
    u = np.multiply(q, x).sum(axis=2) / np.multiply(q, q).sum(axis=2)
    return np.where(
        np.expand_dims((0 <= t) & (t <= 1) & (0 <= u) & (u <= 1), 2),
        x,
        np.full((*t.shape, 2), fill_value=np.nan)
    )

def _segment_intersection_general(p0, p1, q0, q1):
    """
    Finds the intersection of two line segments (p0, p1) and (q0, q1)
    Returns the coordinates of the intersection if it exists, np.ndarray([nan, nan]) otherwise
    """
    p0 = np.expand_dims(p0, 0)
    p1 = np.expand_dims(p1, 0)
    q0 = np.expand_dims(q0, 1)
    q1 = np.expand_dims(q1, 1)
    t = _segment_intersection_nondegenerate(p1 - p0, q1 - q0, q0 - p0)
    return np.where(
        np.isnan(t),
        np.full((2,), np.nan),
        p0 + t * (p1 - p0)
    ).reshape(-1, 2)


def _segment_intersection_degenerate(p, q, b):
    return np.full(p.shape, np.nan)


def _segment_intersection_nondegenerate(p, q, b):
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
                 *,
                 rotation: float = 0,
                 pixfrac: Union[float, tuple[float, float]] = 1,
                 shape: Optional[tuple[int, int]] = None,
                 data: Optional[np.ndarray] = None):
        """
        xlim:
            x limits of the entire grid in grid coordinates
        ylim:
            y limits of the entire grid in grid coordinates
        rotation:
            oriented angle between the grid and world coordinates
        shape:
            the number of pixels in the grid (height, width)
        data:
            array of shape (height, width)
        pixfrac
        """
        self._xmin, self._xmax = xlim
        self._ymin, self._ymax = ylim
        self._rotation = rotation
        if isinstance(pixfrac, float) or isinstance(pixfrac, int):
            self._pixfrac = (pixfrac, pixfrac)
        elif isinstance(pixfrac, tuple):
            self._pixfrac = pixfrac
        else:
            raise TypeError(f"{__name__}: pixfrac must be a float or a 2-tuple of floats")

        if data is None:
            assert shape is not None, "You must provide either shape or explicit initial data"
            self._height, self._width = shape
            self._data = np.zeros(shape=shape)
        else:
            assert shape is None or shape == data.shape,\
                "Provided shape and initial data shape do not match (use shape=None to infer shape from data)"
            assert data.ndim == 2, "Data must form a 2D array"
            self._data = data
            self._height, self._width = data.shape

    @staticmethod
    def from_centre(centre: tuple[float, float],
                    size: tuple[float, float],
                    *,
                    rotation: float = 0,
                    pixfrac: Union[float, tuple[float, float]] = 1,
                    shape: tuple[int, int] = None,
                    data: Optional[np.ndarray] = None):
        cx, cy = centre
        sx, sy = size
        return Grid((cx - sx / 2, cx + sx / 2),
                    (cy - sy / 2, cy + sy / 2),
                    rotation=rotation, shape=shape, data=data, pixfrac=pixfrac)

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
                       pixfrac: Union[float, tuple[float, float]] = (1, 1)):
        """
        Returns a 5D array of pixel boundaries W×H×2×2×2, whose indices are
            vertical coordinate of the pixel
            horizontal coordinate of the pixel
            vertical component of each pixel rectangle (0: bottom, 1: top)
            horizontal component of each pixel rectangle (0: left, 1: right)
            coordinate (0: x, 1: y)
        so that R[5][7][0][1][1] is the y coordinate of the top-left corner of the grid pixel at (7, 5).
        Numpy uses row-major order, so we are keeping it
        """
        xmin, xmax = xlim
        ymin, ymax = ylim
        xcount, ycount = count
        if isinstance(pixfrac, float) or isinstance(pixfrac, int):
            xpixfrac, ypixfrac = pixfrac, pixfrac
        else:
            xpixfrac, ypixfrac = pixfrac

        xsize, ysize = (xmax - xmin) / xcount, (ymax - ymin) / ycount

        centres = Grid.pixel_centres(xlim, ylim, count)
        bottom_left     = centres + np.array((-xsize * xpixfrac / 2, -ysize * ypixfrac / 2))
        bottom_right    = centres + np.array(( xsize * xpixfrac / 2, -ysize * ypixfrac / 2))
        top_left        = centres + np.array((-xsize * xpixfrac / 2,  ysize * ypixfrac / 2))
        top_right       = centres + np.array(( xsize * xpixfrac / 2,  ysize * ypixfrac / 2))
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
        return self._height, self._width

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
        return (self.pixel_height, self.pixel_width)

    @property
    def rotation(self):
        return self._rotation

    def grid_centres(self):
        return self.pixel_centres(
            (self._xmin, self._xmax),
            (self._ymin, self._ymax),
            (self.width, self.height),
        )

    def grid_vertices(self):
        return self.pixel_vertices(
            (self._xmin, self._xmax),
            (self._ymin, self._ymax),
            (self.width, self.height),
            pixfrac=self._pixfrac,
        )

    def world_centres(self):
        mat = np.expand_dims(self.rot_matrix(self.rotation), (0, 1))
        pix = self.grid_centres()
        return np.squeeze(mat @ np.expand_dims(pix, 3), axis=3)

    def world_vertices(self):
        mat = self.rot_matrix(self.rotation)
        pix = self.grid_vertices()
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

    def _overlap_aligned(self, other: Self) -> np.ndarray[float]:
        assert np.abs(np.fmod(self.rotation - other.rotation, math.tau / 4)) < 1e-12,\
            f"{__name__} should not be used when rotations are not the same"

        model = self.grid_centres().reshape((-1, 2))
        data = other.grid_centres().reshape((-1, 2))
        delta = model[:, np.newaxis] - data[np.newaxis, :]

        return (
            self._overlap_single(delta[..., 0], self.pixel_width, other.pixel_width) *
            self._overlap_single(delta[..., 1], self.pixel_height, other.pixel_height)
        ).reshape((*self.shape, *other.shape))

    @staticmethod
    def _overlap_single(delta, data, model) -> np.ndarray[float]:
        """
        Calculate overlap of two line segments of lengths "data" and "model" in one dimension
        when their centres are separated by "delta"
        Adapted from oczoske/lms_reconst
        """
        intercept = (data + model) / (2 * model)
        slope = -1.0 / model
        return np.clip(intercept + slope * np.abs(delta), 0.0, 1.0)

    def _overlap_generic(self, other: Self) -> np.ndarray[float]:
       # assert np.abs(np.fmod(self.rotation - other.rotation, math.tau / 4)) >= 1e-12, \
       #     f"{__name__} should not be used when rotations are very similar to each other"

        result = intersect_rectangles(
            self.world_vertices().reshape(-1, 2, 2, 2),
            other.world_vertices().reshape(-1, 2, 2, 2),
        ).reshape((*self.shape, *other.shape))

        assert result.shape == (self.height, self.width, other.height, other.width),\
            f"{__name__} returns wrong shape {result.shape},"\
            f"should be {(self.height, self.width, other.height, other.width)}"
        return result

    def __matmul__(self, other: Self) -> np.ndarray[float]:
        """
        Project this grid onto another grid and return a 4D overlap matrix
        """
        if np.abs(np.fmod(self.rotation - other.rotation, math.tau / 4)) < 1e-12:
            # When rectangles are aligned, use the simple algorithm
            return self._overlap_generic(other)
        else:
            # When rectangles are not aligned, use the generic approach
            return self._overlap_generic(other)
