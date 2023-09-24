import warnings

import numpy as np

from linesegment import _segment_intersection_general


SLACK = 1e-14


def intersect_rectangles(r1: np.ndarray[float], r2: np.ndarray[float]) -> np.ndarray[float]:
    """
    Takes two arrays of rectangles and computes the area of their overlap
    Works when none of the edges are aligned
    """
    assert r1.shape[-3:] == (2, 2, 2), f"r1 shape is {r1.shape}, expected (..., 2, 2, 2)"
    assert r2.shape[-3:] == (2, 2, 2), f"r2 shape is {r2.shape}, expected (..., 2, 2, 2)"

    # Find all vertices of one rectangle that lie within the other...
    a = np.stack([
        _is_inside_rectangle(r1[..., 0, 0, :],
                             r1[..., 0, 1, :],
                             r1[..., 1, 0, :],
                             r2[..., (x & 2) >> 1, x & 1, :])
        for x in range(0, 4)
    ])
    # ...and vice versa. To preserve the order we have to swap axes in the end
    b = np.stack([
        _is_inside_rectangle(r2[..., 0, 0, :],
                             r2[..., 0, 1, :],
                             r2[..., 1, 0, :],
                             r1[..., (x & 2) >> 1, x & 1, :])
        for x in range(0, 4)
    ]).swapaxes(1, 3).swapaxes(2, 4)
    # Find nontrivial intersections of all pairs of edges
    # The index cycles through the edges of both rectangles in a clever way
    # See https://ksvi.mff.cuni.cz/~kryl/dokumentace.htm#koment, last line:
    # "Definitely add comments to lines where you are proud of how clever your solution is.
    # From my own experience, it pays off to start looking for insidious bugs right there."
    intersections = np.stack([
        _segment_intersection_general(r1[..., (      x & 2) >> 1, ((x + 1) & 2) >> 1, :],
                                      r1[..., ((x + 1) & 2) >> 1, ((x + 2) & 2) >> 1, :],
                                      r2[..., (      x & 8) >> 3, ((x + 4) & 8) >> 3, :],
                                      r2[..., ((x + 4) & 8) >> 3, ((x + 8) & 8) >> 3, :])
        for x in range(0, 16)
    ])

    # Add all 4 + 4 + 16 intersections points together, at most 8 of them are not nan
    vertices = np.concatenate((a, b, intersections), axis=0)

    # Shift the frame to centre of mass, which is guaranteed to be inside the (convex) polygon
    with warnings.catch_warnings():
        warnings.simplefilter("ignore", category=RuntimeWarning)
        centre = np.nanmean(vertices, axis=0)

    shifted = vertices - centre
    # Sort the vertices by azimuth from the centre of mass
    azimuths = np.arctan2(shifted[..., 1], shifted[..., 0])
    shifted = np.take_along_axis(shifted, azimuths.argsort(axis=0)[..., np.newaxis], axis=0)
    # Replace all empty vertices with the zeroth vertex
    has_intersection = np.expand_dims(np.any(~np.isnan(shifted), axis=5), 5)
    shifted = np.where(has_intersection, shifted, shifted[0])
    # Compute the area from the Green's theorem
    shoelace = 0.5 * np.abs(
        np.nansum(
            shifted[..., 0] * (np.roll(shifted[..., 1], -1, axis=0) - np.roll(shifted[..., 1], 1, axis=0)),
            axis=0
        )
    )
    return shoelace


def _is_inside_rectangle(o, p, q, x):
    """
    Determine whether point x is within the rectangle determined by points o, p, q,
    that is (o, p, q, p+q-o) = o + (0, p, q, p+q). We quietly assume that the points
    actually form a rectangle.
    """
    # Expand o, p and q by dimension of x (in the front) and x by dimensions of opq in the back,
    # except the last coordinate
    insert_opq = tuple(range(x.ndim - 1))
    insert_x = tuple(range(x.ndim - 1, x.ndim + o.ndim - 2))
    o = np.expand_dims(o, insert_opq)
    p = np.expand_dims(p, insert_opq)
    q = np.expand_dims(q, insert_opq)
    x = np.expand_dims(x, insert_x)
    return (o + _is_inside_rectangle_origin(p - o, q - o, x - o))


def _is_inside_rectangle_origin(p, q, x) -> np.ndarray[float]:
    """
    Determine whether point x is within the oriented parallelogram determined by vectors p and q
    Returns x if True, np.ndarray([nan, nan]) if false
    """
    t = np.multiply(p, x).sum(axis=-1) / np.multiply(p, p).sum(axis=-1)
    u = np.multiply(q, x).sum(axis=-1) / np.multiply(q, q).sum(axis=-1)
    return np.where(
        #np.expand_dims((0 <= t) & (t <= 1) & (0 <= u) & (u <= 1), -1),
        np.expand_dims((-SLACK <= t) & (t <= 1 + SLACK) & (-SLACK <= u) & (u <= 1 + SLACK), -1),
        x,
        np.full((*t.shape, 2), fill_value=np.nan)
    )


def euclidean_distance(p: np.ndarray[float], q: np.ndarray[float]) -> np.ndarray[float]:
    """
    Determine the distance of two points
    p: (..., d)
    q: (..., d)
    """
    insert_p = tuple(range(q.ndim - 1))
    insert_q = tuple(range(q.ndim - 1, p.ndim + q.ndim - 2))
    p = np.expand_dims(p, insert_p)
    q = np.expand_dims(q, insert_q)
    return np.sqrt(np.sum(np.square(p - q), axis=-1))