# Решаем u_t+a*u_x = 0 явной симметричной по пространству схемой
#
# 1) Решаем задачу на отрезке [-10,10]
# 2) Количество интервалов N = 100, 1000 (два варианта)
# 3) h = 20/N
# 4) a = 1
# 5) tau = 0.25h, 0.5h, h, 1.25h
# 6) Начальные данные: 5 при x<0; 1 при x>0; 3 при x = 0
# 7) Привести картинки численного решения уравнения u_t+u_x =0 на момент времени T = 1. Сплошной линией нарисовать точное решение.
#
# Всего у вас будет 2*4 = 8 картинок
import matplotlib.pyplot as plt
import numpy as np


max_iter = 201

def f(x):
    if x < 0:
        return 5
    elif x > 0:
        return 1
    else:
        return 3


def process_godunov(func, a, x_min, x_max, N, h, tau):
    space = np.linspace(x_min, x_max, N + 1)


    result = np.zeros((max_iter + 1, N + 1))

    for i in range(N + 1):
        result[0][i] = func(space[i])
    # result = {0 : np.array([func(i) for i in space])}

    for i in range(max_iter):

        result[i+1][0] = func(x_min)
        for j in range(1, N + 1):
            result[i+1][j] = result[i][j] - a * tau / h * (result[i][j] - result[i][j - 1])



    return space, result


def process_symmetrical(func, a, x_min, x_max, N, h, tau):
    space = np.linspace(x_min, x_max, N + 1)


    result = np.zeros((max_iter + 1, N + 1))

    for j in range(N + 1):
        result[0][j] = func(space[j])
    # result = {0 : np.array([func(i) for i in space])}

    for n in range(max_iter):

        result[n+1][0] = func(x_min)
        result[n+1][N] = func(x_max)

        for j in range(1, N):
            result[n+1][j] = result[n][j] - a * tau / (2 * h) * (result[n][j + 1] - result[n][j - 1])



    return space, result



taus_mult = [0.25, 0.5, 1, 1.25]
Ns = [100, 1000]

# x2 = np.linspace(0, 10, 101)
# y1_true = [f(i - 1) for i in x1]

for N in Ns:
    x = np.linspace(-10, 10, N + 1)
    y_true = [f(i - 1) for i in x]

    for tau_mult in taus_mult:


        h = 20.0 / N
        tau = tau_mult * h
        x, ys = process_symmetrical(f, 1, -10, 10, N, h, tau)
        t = int(1 / tau)

        # plt.clf()
        plt.plot(x, y_true, color='red', label="true")
        plt.title(f"tau = {tau_mult} * h, N = {N}")
        plt.plot(x, ys[t], linestyle='--', label=f"tau = {tau_mult} * h")
        plt.savefig(f"tau_mult_{tau_mult}_N_{N}.jpg")
        plt.show()




