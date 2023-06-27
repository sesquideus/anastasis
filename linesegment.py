import numpy as np


# Slack constant: to avoid false negatives in intersections, change condition from
# 0 <= t <= 1 to -SLACK <= t <= 1 + SLACK. False positives add negligible errors.
SLACK = 1e-14


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
    insert_p = tuple(range(q0.ndim - 1))
    insert_q = tuple(range(q0.ndim - 1, p0.ndim + q0.ndim - 2))
    p0 = np.expand_dims(p0, insert_p)
    p1 = np.expand_dims(p1, insert_p)
    q0 = np.expand_dims(q0, insert_q)
    q1 = np.expand_dims(q1, insert_q)
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
    t = np.divide(np.cross(b, q), det, where=np.abs(det) > SLACK, out=np.full_like(det, np.nan))
    u = np.divide(np.cross(b, p), det, where=np.abs(det) > SLACK, out=np.full_like(det, np.nan))
    t = np.expand_dims(t, t.ndim)
    u = np.expand_dims(u, u.ndim)
    return np.where(
        (np.expand_dims(det, det.ndim) != 0) & (-SLACK <= t) & (t <= 1 + SLACK) & (-SLACK <= u) & (u <= 1 + SLACK),
        t,
        np.nan
    )
