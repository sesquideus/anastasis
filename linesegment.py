import numpy as np


def segment_intersection(p0, p1, q0, q1):
    p = p1 - p0
    q = q1 - q0
    b = q0 - p0
    det = np.cross(p, q)
    return np.where(
        np.expand_dims(np.abs(det) < 1e-15, det.ndim),
        _segment_intersection_degenerate(p, q, b),
        p0 + _segment_intersection_nondegenerate(p, q, b) * p,
    )

def _segment_intersection_general(p0, p1, q0, q1):
    """
    Finds the intersection of two line segments (p0, p1) and (q0, q1)
    Returns the coordinates of the intersection if it exists, np.ndarray([nan, nan]) otherwise
    """
    p0 = np.expand_dims(p0, (0, 1))
    p1 = np.expand_dims(p1, (0, 1))
    q0 = np.expand_dims(q0, (2, 3))
    q1 = np.expand_dims(q1, (2, 3))
    t = _segment_intersection_nondegenerate(p1 - p0, q1 - q0, q0 - p0)
    # return the intersection point if it exists or a 2-tuple of nans if there is no intersection
    return np.where(
        np.isnan(t),
        np.full((2,), np.nan),
        p0 + t * (p1 - p0)
    )


def _segment_intersection_degenerate(p, q, b):
    return np.full(p.shape, np.nan)


def _segment_intersection_nondegenerate(p, q, b):
    det = np.cross(p, q).astype(float)
    t = np.divide(np.cross(b, q), det, where=np.abs(det) > 1e-15, out=np.full_like(det, np.nan))
    u = np.divide(np.cross(b, p), det, where=np.abs(det) > 1e-15, out=np.full_like(det, np.nan))
    t = np.expand_dims(t, t.ndim)
    u = np.expand_dims(u, u.ndim)
    return np.where((np.expand_dims(det, det.ndim) != 0) & (-1e-15 <= t) & (t <= 1 + 1e-15) & (-1e-15 <= u) & (u <= 1 + 1e-15), t, np.nan)
