#!/usr/bin/env python3

import argparse
import matplotlib
import numpy as np

from matplotlib import pyplot as plt
from pathlib import Path


class Heatmap():
    def __init__(self):
        self.argparser = argparse.ArgumentParser(
            description="Reconstruction file visualizer"
        )
        self.cmap_arg = self.argparser.add_argument('-c', '--cmap', type=str, default='viridis')
        self.infile_arg = self.argparser.add_argument('infile', type=argparse.FileType('r'), help="Binary input file")
        self.outfile_arg = self.argparser.add_argument('-o', '--outfile', type=argparse.FileType('w'), help="PNG output file")
        self.w_arg = self.argparser.add_argument('w', type=int)
        self.h_arg = self.argparser.add_argument('h', type=int)
        self.args = self.argparser.parse_args()

        self.file = Path(self.args.infile.name)

        if self.args.outfile:
            matplotlib.use('Agg')
            self.outfile = Path(self.args.outfile.name)
        else:
            matplotlib.use('TkAgg')

        self.load()
        self.plot()

class Heatmap2D(Heatmap):

    def load(self):
        self.data = np.fromfile(self.file, sep='', dtype=float)
        self.data = self.data.reshape(self.args.h, self.args.w)

    def plot(self):
        fig = plt.figure(1, figsize=(16, 16))
        ax = fig.subplots(1, 1)
        ax.set_frame_on(False)
        #ax.grid(color='black', linestyle='-', alpha=1)
        #ax.xaxis.set_major_locator(matplotlib.ticker.MultipleLocator(base=8))
        #ax.yaxis.set_major_locator(matplotlib.ticker.MultipleLocator(base=8))
        ax.set_xticklabels([])
        ax.set_yticklabels([])
        ax.set_aspect('equal')

        data = ax.imshow(self.data, cmap=self.args.cmap, vmin=0, extent=[0, self.args.w, 0, self.args.h], origin='lower')
        fig.colorbar(data)

        plt.tight_layout(pad=0)
        if self.args.outfile:
            fig.savefig(self.outfile, dpi=128, pad_inches=0, format='png')
        else:
            plt.show()

        plt.close('all')


Heatmap2D().load()
