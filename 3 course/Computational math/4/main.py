import numpy as np


A = np.array(
    [[1, 1, 1, 1, 4], [6, 1, 2, 1, 10], [2, 3, 1, 6, 12], [1, 1, 4, 1, 7]],
    dtype="float64",
)


for i in range(4):
    j = i + np.argmax(A[i:, i])

    tmp = A[i].copy()
    A[i] = A[j]
    A[j] = tmp
    A[i] = A[i] / A[i, i]

    for k in range(i + 1, 4):
        A[k] -= A[i] * A[k][i]

    print(A)
    print()

x = np.array([0, 0, 0, 0], dtype="float64")
for i in range(3, -1, -1):
    coef = A[i, i + 1 : -1]
    x[i] = A[i, -1] - np.sum(coef * x[i + 1 :])
print(x)
