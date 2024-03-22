#!/usr/bin/env python

import argparse
import numpy as np
from matplotlib import pyplot as plt


parser = argparse.ArgumentParser()
parser.add_argument('infile', type=argparse.FileType('rb'))
args = parser.parse_args()

y = np.load(args.infile)
plt.imshow(y, origin='lower', cmap='grey', vmin=0)
plt.show()
