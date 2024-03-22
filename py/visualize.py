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
        self.cmap_arg = self.argparser.add_argument('-c', '--cmap', type=str, default='grey')
        self.infile_arg = self.argparser.add_argument('infile', type=argparse.FileType('r'), help="Binary input file")
        self.reffile_arg = self.argparser.add_argument('-r', '--reference-file', type=argparse.FileType('r'), help="Binary input file")
        self.outfile_arg = self.argparser.add_argument('-o', '--outfile', type=argparse.FileType('w'), help="PNG output file")
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
        self.data = np.load(self.file)

    def plot(self):
        fig = plt.figure(1, figsize=(16, 16))
        ax = fig.subplots(1, 1)
        ax.set_frame_on(False)
        ax.set_xticklabels([])
        ax.set_yticklabels([])
        ax.set_aspect('equal')

        data = ax.imshow(self.data, vmin=0, origin='lower', cmap=self.args.cmap)
        fig.colorbar(data)

        plt.tight_layout(pad=0)
        if self.args.outfile:
            fig.savefig(self.outfile, dpi=128, pad_inches=0, format='png')
        else:
            plt.show()

        plt.close('all')


Heatmap2D().load()
