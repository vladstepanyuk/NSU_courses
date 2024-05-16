from sklearn.preprocessing import OneHotEncoder
from layers import nn
from metrics import metrics
from sklearn.model_selection import train_test_split
import pandas as pd
import numpy as np


def main():
    a = np.array([[[[1, 2, 3], [4, 5, 6], [7, 8, 9]], [[1, 2, 3], [4, 5, 6], [7, 8, 9]]]], dtype='float32')
    xa = a.shape[-2]
    ya = a.shape[-1]

    b = np.array([[[[1, 0]], [[0, 1]]], [[[0, 1]], [[0, 0]]]], dtype='float32')
    xb = b.shape[-2]
    yb = b.shape[-1]
    print(b.shape)

    pb = np.pad(b, ((0, 0), (0, 0), (0, xa - xb), (0, ya - yb)), mode='constant', constant_values=0)
    print(a)
    print(pb)

    flatpb = np.reshape(pb, newshape=(2, -1))
    flata = np.reshape(a, newshape=(a.shape[0], -1))
    newx = xa - xb + 1
    newy = ya - yb + 1

    newsize = newx * newy
    mat = np.zeros(shape=(2, a.size // a.shape[0], newsize))

    tmp = flatpb.copy()
    # for j in range(mat.shape[0]):
    #     for x_t in range(newx):
    #         for y_t in range(newy):
    #             mat[j, :, y_t + x_t * newy] = tmp[j]
    #             tmp[j] = np.roll(tmp[j], 1)
    #         tmp[j] = np.roll(tmp[j], newy - 1)
    print(mat)

    print(flatpb)

    print(flata.shape)
    print(mat.shape)
    res = flata @ mat
    print(res.shape)
    res = np.transpose(res, (1, 0, 2))

    print(np.reshape(res, newshape=(a.shape[0], 2, newx, newy)))


if __name__ == '__main__':
    main()
