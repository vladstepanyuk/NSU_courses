import numpy as np


def calc_integral(f, a, b, k=10):
    """
    Calculate the definite integral of a given function using the trapezoidal rule.
    Parameters:
        f (function): The function to integrate.
        a (float): The lower limit of integration.
        b (float): The upper limit of integration.
        k (int, optional): The number of intervals to divide the interval [a, b] into. Default is 10.
    Returns:
        float: The value of the definite integral.
    """

    x = np.linspace(a, b, k+1)

    integral = 0
    for i in range(k):
        integral += (x[i + 1] - x[i]) * (f(x[i]) + f(x[i + 1])) / 2

    return integral



def experiment(k):
    S_h = calc_integral(np.exp, 0, 1, k=k)
    S_h2 = calc_integral(np.exp, 0, 1, k=2 * k)
    print(f"k = {k}")
    print(f"S_h = {S_h}")
    print(f"S_h2 = {S_h2}")
    I = np.e - 1
    print(f"порядок интегрирования ~ {np.log2(abs(I - S_h) / abs(I - S_h2))}")

k = 10

experiment(k)

k = 100

experiment(k)

