import numpy as np
import matplotlib.pyplot as plt
from concurrent.futures import ThreadPoolExecutor, as_completed
import sys
def Y(x):
    return 1 if x > 0 else -1
def calculate_color(args):
    i, j, x, y, w1, w2, b = args
    r = Y(x * w1 + y * w2 + b)
    green, red = (1, 0) if r == 1 else (0, 1)
    blue = 0
    return [red, green, blue], i, j
def generateImage(image, coords):
    executor = ThreadPoolExecutor(max_workers=1000)
    futures = [executor.submit(calculate_color, coord) for coord in coords]

    for future in as_completed(futures):
        result, i, j = future.result()
        image[i, j, :] = result


def generateTable(title, filename, w1, w2, b):
    resolution = 100
    colors = np.zeros((resolution, resolution, 3))
    x = np.linspace(-1, 1, resolution)
    y = np.linspace(-1, 1, resolution)
    coords = [(i, j, x[i], y[j], w1, w2, b) for i in range(resolution) for j in range(resolution)]
    generateImage(colors, coords)
    plt.imshow(colors, extent=[-1, 1, -1, 1])
    plt.xlabel('X1')
    plt.ylabel('X2')
    plt.title(title)
    # plt.colorbar()
    # plt.show()
    plt.savefig(filename)


if __name__ == "__main__":
    if len(sys.argv) == 6:
        print('python calculate', sys.argv)
        _, title, fileout, w1, w2, b = sys.argv
        w1, w2, b = float(w1), float(w2), float(b)
        generateTable(title, fileout, w1, w2, b)
