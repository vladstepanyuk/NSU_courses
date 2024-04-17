import unittest

import torch
from torch.autograd import Variable
import numpy as np

from nn import Linear
from nn import Relu


class TestNNLayers(unittest.TestCase):

    def calculate_metric(self, X1: np.ndarray, X2: np.ndarray):
        err = (X1 - X2)
        return np.sum(err * err)

    def test_linear(self):
        ll1 = Linear(3, 3)
        ll2 = Linear(3, 4)
        x = np.array([1, 2, 3], dtype="float64").reshape((3, 1))
        y_true = np.array([1, 2, 3, 4], dtype="float64").reshape(4, 1)
        y_pred = ll2.forward(ll1.forward(x))

        loss = (y_true - y_pred).T @ (y_true - y_pred)

        grad = - 2 * (y_true - y_pred)
        ll1.backward(ll2.backward(grad))

        A1_t = Variable(torch.from_numpy(ll1.params), requires_grad=True)
        A2_t = Variable(torch.from_numpy(ll2.params), requires_grad=True)
        x_t = Variable(torch.from_numpy(x))
        y_t = torch.matmul(A2_t, torch.matmul(A1_t, x_t))

        y_true_t = Variable(torch.from_numpy(y_true))

        loss_t = torch.matmul((y_true_t - y_t).T, y_true_t - y_t)
        loss_t.backward()

        self.assertAlmostEqual(self.calculate_metric(A1_t.grad.numpy(), ll1.grad), 0)
        self.assertAlmostEqual(self.calculate_metric(A2_t.grad.numpy(), ll2.grad), 0)
        self.assertAlmostEqual(self.calculate_metric(y_t.detach().numpy(), y_pred), 0)
        self.assertAlmostEqual(self.calculate_metric(loss_t.detach().numpy(), loss), 0)

    def test_relu(self):
        ll = Linear(3, 4)
        relul = Relu()
        x = np.array([1, 1, 1], dtype="float64").reshape((3, 1))
        y_true = np.array([1, 2, 3, 4], dtype="float64").reshape(4, 1)
        y_pred = relul.forward(ll.forward(x))

        loss = (y_true - y_pred).T @ (y_true - y_pred)

        grad = - 2 * (y_true - y_pred)
        ll.backward(relul.backward(grad))

        A_t = Variable(torch.from_numpy(ll.params), requires_grad=True)
        x_t = Variable(torch.from_numpy(x))
        relu_t = torch.nn.ReLU()
        y_t = relu_t.forward(torch.matmul(A_t, x_t))
        y_true_t = Variable(torch.from_numpy(y_true))

        loss_t = torch.matmul((y_true_t - y_t).T, y_true_t - y_t)
        loss_t.backward()

        self.assertAlmostEqual(self.calculate_metric(A_t.grad.numpy(), ll.grad), 0)
        self.assertAlmostEqual(self.calculate_metric(y_t.detach().numpy(), y_pred), 0)
        self.assertAlmostEqual(self.calculate_metric(loss_t.detach().numpy(), loss), 0)


if __name__ == '__main__':
    unittest.main()
