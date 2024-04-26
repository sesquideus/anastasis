#!/usr/bin/env python

import argparse
import numpy as np
from matplotlib import pyplot as plt


parser = argparse.ArgumentParser()
parser.add_argument('infile', type=argparse.FileType('rb'))
args = parser.parse_args()

y = np.load(args.infile)
image = plt.imshow(y, origin='lower', cmap='grey')
plt.colorbar(image)
plt.show()
