import numpy as np

from typing import Self


class Rect:
    def __init__(self, cx: float, cy: float, w: float, h: float, rot: float):
        self.cx = cx
        self.cy = cy
        self.w = w
        self.h = h
        self.rot = rot

    @property
    def points(self) -> np.ndarray[2, 2, 2]:
        tl = (-self.w / 2, self.h / 2)

        return self.cx - self.w / 2


    def __matmul__(self, other: Self) -> float:
        pass
