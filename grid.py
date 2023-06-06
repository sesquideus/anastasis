import math
import numpy as np

from typing import Optional, Self, Union

from linesegment import _segment_intersection_general
from rectangle import intersect_rectangles


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
        pixfrac:
            scaling factor for pixels, either a single float or 2-tuple
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
            [np.cos(angle), np.sin(angle)],
            [-np.sin(angle), np.cos(angle)]
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

    @property
    def left(self):
        return self._xmin

    @property
    def right(self):
        return self._xmax

    @property
    def bottom(self):
        return self._ymin

    @property
    def top(self):
        return self._ymax

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
        a = np.swapaxes(mat @ np.swapaxes(pix, 3, 4), 3, 4)
      #  print("Rotated matrix:", a)
        return a

    def __str__(self):
        return f"Grid {self.width}×{self.height}, {self._xmin} to {self._xmax}, rotated by {self.rotation:.6f}"

    def _print(self, func):
        vertices = self.world_vertices()
        for row in range(self.height):
            for col in range(self.width):
                print(vertices)
                rect = vertices[row, col]
                for v in [1, 0]:
                    for h in [0, 1]:
                        print(f"{h} {v} {rect[v][h][0]:+.6f}, {rect[v][h][1]:+.6f}", end=' | ' if h == 0 else '')
                    print()

    def print_grid(self):
        return self._print(self.grid_vertices())

    def print_world(self):
        return self._print(self.world_vertices())

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

        return intersect_rectangles(
            other.world_vertices(),
            self.world_vertices(),
        )

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
