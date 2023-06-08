import math
import numpy as np

from typing import Optional, Self, Union

from linesegment import _segment_intersection_general
from rectangle import intersect_rectangles


class Grid:
    def __init__(self,
                 centre: tuple[float, float],
                 extent: tuple[float, float],
                 *,
                 rotation: float = 0,
                 pixfrac: Union[float, tuple[float, float]] = 1,
                 shape: Optional[tuple[int, int]] = None,
                 data: Optional[np.ndarray] = None):
        """
        centre:
            x, y coordinates of the physical centre of the grid
        extent:
            physical size of the entire grid
        rotation:
            oriented angle between the grid and world coordinates (positive anti-clockwise)
        shape:
            the number of pixels in the grid (height, width)
        data:
            optional, pixel values array of shape (height, width)
        pixfrac:
            scaling factor for pixels (horizontal, vertical), either a single float or 2-tuple
        """
        self._cx, self._cy = centre
        self._pwidth, self._pheight = extent
        assert self.physical_width > 0, f"Physical width of the grid must be > 0, is {self.physical_width}"
        assert self.physical_height > 0, f"Physical height of the grid must be > 0, is {self.physical_height}"

        self._rotation = rotation

        if isinstance(pixfrac, float) or isinstance(pixfrac, int):
            self._pixfrac_w = pixfrac
            self._pixfrac_h = pixfrac
        elif isinstance(pixfrac, tuple):
            self._pixfrac_w, self._pixfrac_h = pixfrac
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
    def cx(self):
        """
        Centre of the physical grid, x coordinate
        """
        return self._cx

    @property
    def cy(self):
        """
        Centre of the physical grid, y coordinate
        """
        return self._cy

    @property
    def centre(self) -> np.ndarray[float]:
        """
        Coordinates of the centre of the grid
        """
        return np.array([self.cx, self.cy], dtype=float)

    @property
    def physical_width(self):
        """
        Physical width of the grid
        """
        return self._pwidth

    @property
    def physical_height(self):
        """
        Physical height of the grid
        """
        return self._pheight

    @property
    def width(self) -> int:
        """
        Number of pixels in horizontal direction (grid coordinates)
        """
        return self._width

    @property
    def height(self) -> int:
        """
        Number of pixels in vertical direction (grid coordinates)
        """
        return self._height

    @property
    def shape(self):
        return self._height, self._width

    @property
    def size(self):
        return self._width * self._height

    @property
    def left(self):
        """
        Left border in grid coordinates
        """
        return -self.physical_width / 2.0

    @property
    def right(self):
        """
        Right border in grid coordinates
        """
        return self.physical_width / 2.0

    @property
    def bottom(self):
        """
        Bottom border in grid coordinates
        """
        return -self.physical_height / 2.0

    @property
    def top(self):
        """
        Top border in grid coordinates
        """
        return self.physical_height / 2.0

    @property
    def pixfrac_w(self):
        return self._pixfrac_w

    @property
    def pixfrac_h(self):
        return self._pixfrac_h

    @property
    def pixfrac(self) -> tuple[float, float]:
        return (self.pixfrac_w, self.pixfrac_h)

    @property
    def element_width(self):
        return self.physical_width / self.width

    @property
    def element_height(self):
        return self.physical_height / self.height

    @property
    def pixel_width(self):
        return self.element_width * self.pixfrac_w

    @property
    def pixel_height(self):
        return self.element_height * self.pixfrac_h

    @property
    def pixel_size(self):
        return (self.pixel_height, self.pixel_width)

    @property
    def pixel_area(self):
        return self.pixel_height * self.pixel_width

    @property
    def rotation(self):
        return self._rotation

    @property
    def grid_centres(self):
        return self.pixel_centres(
            (self.left, self.right),
            (self.bottom, self.top),
            (self.width, self.height),
        )

    @property
    def grid_vertices(self):
        return self.pixel_vertices(
            (self.left, self.right),
            (self.bottom, self.top),
            (self.width, self.height),
            pixfrac=(self._pixfrac_w, self._pixfrac_h),
        )

    @property
    def world_centres(self):
        mat = np.expand_dims(self.rot_matrix(self.rotation), (0, 1))
        return np.squeeze(mat @ np.expand_dims(self.grid_centres, 3), axis=3) + self.centre

    @property
    def world_vertices(self):
        mat = self.rot_matrix(self.rotation)
        vertices = self.grid_vertices
        return np.swapaxes(mat @ np.swapaxes(vertices, 3, 4), 3, 4) + self.centre

    def __str__(self):
        return f"Grid at {self.cx}, {self.cy} of physical size {self.width}×{self.height}, "\
            f"shape {self.shape}, rotated by {self.rotation:.6f}"

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
        return self._print(self.grid_vertices)

    def print_world(self):
        return self._print(self.world_vertices)

    def _overlap_aligned(self, other: Self) -> np.ndarray[float]:
        assert np.abs(np.fmod(self.rotation - other.rotation, math.tau / 4)) < 1e-12,\
            f"{__name__} must not be used when rotations are not the same"

        model = self.grid_centres
        data = other.grid_centres
        delta = np.expand_dims(model, (2, 3)) - np.expand_dims(data, (0, 1))
        return (
            self._overlap_single(delta[..., 0], self.pixel_width, other.pixel_width) *
            self._overlap_single(delta[..., 1], self.pixel_height, other.pixel_height)
        )

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

        return intersect_rectangles(other.world_vertices, self.world_vertices) / other.pixel_area

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
