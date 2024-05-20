#!/usr/bin/env python

import argparse
import numpy as np

from matplotlib import pyplot as plt
from matplotlib import colors

parser = argparse.ArgumentParser()
parser.add_argument('infile', type=argparse.FileType('rb'))
parser.add_argument('width', type=int, default=0)
parser.add_argument('height', type=int, default=0)
parser.add_argument('--two-sloped', '-t', action='store_true')
parser.add_argument('--title', '-T', action='store_true')
parser.add_argument('--output', '-o', type=argparse.FileType('wb'))
args = parser.parse_args()

y = np.load(args.infile)
width = args.width or y.shape[1]
height = args.height or y.shape[0]

if args.two_sloped:
    cmap = 'bwr'
    norm = colors.TwoSlopeNorm(0)
else:
    cmap = 'grey'
    norm = colors.Normalize(vmin=0)

plt.rcParams.update({'font.size': 28})
plt.figure(figsize=(16, 12))

image = plt.imshow(y, origin='lower', cmap=cmap, norm=norm, extent=[0, width, 0, height])

if args.title:
    plt.title(args.infile.name)

plt.colorbar(image)

if args.output:
    plt.tight_layout()
    plt.savefig(args.output, dpi=100, pad_inches=-0.1)
else:
    plt.show()
