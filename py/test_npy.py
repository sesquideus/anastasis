import numpy as np
from matplotlib import pyplot as plt


x = np.ones(shape=(19, 17), dtype=np.float32)

np.save('numpy.npy', x, allow_pickle=False)


y = np.load('../cpp/cmake-build-release/out/drizzled.npy')

plt.imshow(y, origin='lower', cmap='grey')
plt.show()
