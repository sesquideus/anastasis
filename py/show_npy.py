#!/usr/bin/env python

import argparse
import numpy as np
from matplotlib import pyplot as plt


parser = argparse.ArgumentParser()
parser.add_argument('original', type=argparse.FileType('rb'))
parser.add_argument('infile', type=argparse.FileType('rb'))
args = parser.parse_args()

fig, axes = plt.subplots(1, 2)

original = np.load(args.original)
drizzled = np.load(args.infile)
axes[0].set_title("Original")
axes[0].imshow(original, origin='lower', cmap='grey', vmin=0)
axes[1].set_title("Drizzled")
axes[1].imshow(drizzled, origin='lower', cmap='grey', vmin=0)

plt.show()
