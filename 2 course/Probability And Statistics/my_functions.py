import numpy as np
import scipy.stats as stats

error = 0.0001
error_square = error ** 2
MAX_ITER = 1000


def find_quantile(epsilon: float, dist_func):
    a_min = 0
    a_max = 0
    while dist_func(a_max) < 1 - error:
        a_max += 1000

    while dist_func(a_min) > error:
        a_min -= 1000

    a = (a_max + a_min) / 2

    for i in range(MAX_ITER):
        if (dist_func(a) - epsilon) ** 2 < error_square:
            break

        if dist_func(a) > epsilon:
            a_max = a
        else:
            a_min = a

        a = (a_max + a_min) / 2

    return a


def ecdf(data):
    unique, counts = np.unique(data, return_counts=True)
    p = counts / len(data)

    pc = p
    for i in range(1, len(p)):
        pc[i] = pc[i - 1] + p[i]

    return unique, pc


def find_d_n(data: np.ndarray, dist_func):
    unique, p = ecdf(data)
    f_f1 = np.abs(dist_func(unique[1:]) - p[1:])
    f_f2 = np.abs(dist_func(unique[1:]) - p[:-1])
    f = np.maximum(f_f1, f_f2)

    return unique[np.argmax(f)], np.max(f)


def find_rho_pirson(data: np.ndarray, dist_func):
    k = int(np.log2(data.size)) + 1
    n = data.size
    barriers = np.linspace(0, 1, k + 1)
    v = np.zeros(k)
    p = np.zeros(k)
    for i in range(1, k + 1):
        v[i - 1] = np.count_nonzero(np.logical_and((barriers[i] > data), (barriers[i - 1] < data)))
        p[i - 1] = dist_func(barriers[i]) - dist_func(barriers[i - 1])

    rho = np.sum(np.power(v - n * p, 2) / (n * p))

    return rho
