import numpy as np


x = np.ones(shape=(19, 17), dtype=np.float32)

np.save('numpy.npy', x, allow_pickle=False)

