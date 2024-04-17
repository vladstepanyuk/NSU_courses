import numpy as np

import torch
from torch.autograd import Variable

from torch import nn


# np.random.seed(42)


class Function:
    def __init__(self):
        self.params = None
        self.grad = None
        """initialize layer"""

    def forward(self, input: np.ndarray) -> np.ndarray:
        """forward"""

    def backward(self, out_gradient: np.ndarray) -> np.ndarray:
        """backward"""

    def zerograd(self):
        """zerograd"""

    def have_grad(self) -> bool:
        """is layer has gradient"""


class Linear(Function):
    def __init__(self, input_n, output_n, A: np.ndarray = None):
        super().__init__()

        if A is None:
            normal_dist = np.random.normal(size=input_n * output_n, )
            self.params = np.reshape(normal_dist, newshape=(output_n, input_n))
        elif A.shape == (output_n, input_n):
            self.params = A
        else:
            raise "invalid size"
        self.grad = np.zeros(shape=(output_n, input_n))
        self.last_inputT = np.zeros(shape=(1, input_n))

    def forward(self, input: np.ndarray):
        self.last_inputT = input.T
        out = self.params @ input
        return out

    def backward(self, out_gradient: np.ndarray):
        self.grad += out_gradient @ self.last_inputT

        return self.params.T @ out_gradient

    def zerograd(self):
        self.grad.fill(0)

    def have_grad(self) -> bool:
        return True


class Relu(Function):
    def __init__(self):
        super().__init__()
        self.last_input = 0

    def forward(self, input: np.ndarray) -> np.ndarray:
        self.last_input = input

        return input * (input > 0)

    def backward(self, out_gradient: np.ndarray) -> np.ndarray:
        return out_gradient * (self.last_input > 0)

    def have_grad(self) -> bool:
        return False


class Model:

    def __init__(self, layers):
        self.layers = []

        for l in layers:
            self.layers.append(l)

    def forward(self, input: np.ndarray) -> np.ndarray:
        out = input
        for l in self.layers:
            inn = out
            out = l.forward(inn)
        return out

    def backward(self, out_grad: np.ndarray) -> np.ndarray:
        out = out_grad
        for l in self.layers[::-1]:
            inn = out
            out = l.backward(inn)
        return out

    def zerograd(self):
        for l in self.layers:
            l.zerograd()


class LossFunction:
    def __init__(self):
        """init"""

    def calc_loss(self, y_true, y_pred):
        """loss calculation"""

    def calc_gradient(self, y_true, y_pred):
        """gradient calculation"""


class MSE(LossFunction):

    def calc_loss(self, y_true, y_pred):
        err = y_true - y_pred
        return np.sum(err * err)

    def calc_gradient(self, y_true, y_pred):
        err = y_true - y_pred
        return - 2 * err


class Optimizer:
    def __init__(self, lr: float):
        self.lr = lr

    def do_step_layer(self, layer: Function):
        layer.params -= self.lr * layer.grad

    def do_step(self, model: Model):
        for l in model.layers:
            if l.have_grad():
                self.do_step_layer(l)
