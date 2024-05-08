#!/usr/bin/env python

import argparse
import numpy as np
from matplotlib import pyplot as plt


parser = argparse.ArgumentParser()
parser.add_argument('infile', type=argparse.FileType('rb'))
parser.add_argument('width', type=int, default=0)
parser.add_argument('height', type=int, default=0)
args = parser.parse_args()

y = np.load(args.infile)
width = args.width or y.shape[1]
height = args.height or y.shape[0]

image = plt.imshow(y, origin='lower', cmap='grey', extent=[0, width, 0, height])
plt.title(args.infile.name)
plt.colorbar(image)
plt.show()
