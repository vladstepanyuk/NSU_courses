import unittest

import torch
from torch.autograd import Variable
import numpy as np

from nn import Linear
from nn import Relu
from nn import Sigmoid

from nn import CrossEntropy


class TestNNLayers(unittest.TestCase):

    def calculate_metric(self, X1: np.ndarray, X2: np.ndarray):
        err = (X1 - X2)
        return np.sum(err * err)

    def test_linear(self):
        ll1 = Linear(3, 3)
        ll2 = Linear(3, 1)
        x = np.array([1, 2, 3], dtype="float32").reshape((1, 3))
        y_true = np.array([1], dtype="float32").reshape(1, 1)

        y_pred = ll2.forward(ll1.forward(x))

        loss = (y_true - y_pred).T @ (y_true - y_pred)

        grad = - 2 * (y_true - y_pred)
        ll1.backward(ll2.backward(grad))

        A1_t = Variable(torch.from_numpy(ll1.get_params()))
        b1_t = Variable(torch.from_numpy(ll1.get_bias()))
        A2_t = Variable(torch.from_numpy(ll2.get_params()))
        b2_t = Variable(torch.from_numpy(ll2.get_bias()))

        ll1_t = torch.nn.Linear(3, 3)
        ll2_t = torch.nn.Linear(3, 1)
        with torch.no_grad():
            ll1_t.weight.copy_(A1_t)
            ll1_t.bias.copy_(b1_t)
            ll2_t.weight.copy_(A2_t)
            ll2_t.bias.copy_(b2_t)

        x_t = Variable(torch.from_numpy(x))
        y_t = ll2_t.forward(ll1_t.forward(x_t))

        y_true_t = Variable(torch.from_numpy(y_true))

        loss_t = torch.matmul((y_true_t - y_t).T, y_true_t - y_t)
        loss_t.backward()

        ll1_twg = ll1_t.weight.grad.numpy()
        ll1_tbg = ll1_t.bias.grad.numpy()
        ll2_twg = ll2_t.weight.grad.numpy()
        ll2_tbg = ll2_t.bias.grad.numpy()

        ll1_tbg = np.reshape(ll1_tbg, newshape=(ll1_tbg.shape[0], 1))
        ll2_tbg = np.reshape(ll2_tbg, newshape=(ll2_tbg.shape[0], 1))

        ll1_t_g = np.hstack((ll1_twg, ll1_tbg))
        ll2_t_g = np.hstack((ll2_twg, ll2_tbg))

        self.assertAlmostEqual(
            self.calculate_metric(ll1_t_g, ll1.grad), 0)
        self.assertAlmostEqual(
            self.calculate_metric(ll2_t_g, ll2.grad), 0)
        self.assertAlmostEqual(self.calculate_metric(y_t.detach().numpy(), y_pred), 0)
        self.assertAlmostEqual(self.calculate_metric(loss_t.detach().numpy(), loss), 0)

    def test_relu(self):
        ll1 = Linear(3, 1)
        relu = Relu()
        x = np.array([1, 2, 3], dtype="float32").reshape((1, 3))
        y_true = np.array([1], dtype="float32").reshape(1, 1)

        y_pred = relu.forward(ll1.forward(x))

        loss = (y_true - y_pred).T @ (y_true - y_pred)

        grad = - 2 * (y_true - y_pred)
        ll1.backward(relu.backward(grad))

        A1_t = Variable(torch.from_numpy(ll1.get_params()))
        b1_t = Variable(torch.from_numpy(ll1.get_bias()))

        ll1_t = torch.nn.Linear(3, 1)
        relu_t = torch.nn.ReLU()
        with torch.no_grad():
            ll1_t.weight.copy_(A1_t)
            ll1_t.bias.copy_(b1_t)

        x_t = Variable(torch.from_numpy(x))
        y_t = relu_t.forward(ll1_t.forward(x_t))

        y_true_t = Variable(torch.from_numpy(y_true))

        loss_t = torch.matmul((y_true_t - y_t).T, y_true_t - y_t)
        loss_t.backward()

        ll1_twg = ll1_t.weight.grad.numpy()
        ll1_tbg = ll1_t.bias.grad.numpy()

        ll1_tbg = np.reshape(ll1_tbg, newshape=(ll1_tbg.shape[0], 1))

        ll1_t_g = np.hstack((ll1_twg, ll1_tbg))

        self.assertAlmostEqual(
            self.calculate_metric(ll1_t_g, ll1.grad), 0)
        self.assertAlmostEqual(self.calculate_metric(y_t.detach().numpy(), y_pred), 0)
        self.assertAlmostEqual(self.calculate_metric(loss_t.detach().numpy(), loss), 0)

    def test_loss(self):
        ll1 = Linear(3, 1)
        sig = Sigmoid()
        x = np.array([0.1, 10, 0, 0.1, 0.1, 0, 0.1, 1, 0], dtype="float32").reshape((3, 3))
        y_true = np.array([0, 1, 1], dtype="float32").reshape(3, 1)
        loss = CrossEntropy()

        y_pred = sig.forward(ll1.forward(x))

        grad = loss.calc_gradient(y_true, y_pred)

        ll1.backward(sig.backward(grad))

        A1_t = Variable(torch.from_numpy(ll1.get_params()))
        b1_t = Variable(torch.from_numpy(ll1.get_bias()))

        ll1_t = torch.nn.Linear(3, 1)
        sig_t = torch.nn.Sigmoid()
        with torch.no_grad():
            ll1_t.weight.copy_(A1_t)
            ll1_t.bias.copy_(b1_t)

        x_t = Variable(torch.from_numpy(x))
        y_t = sig_t.forward(ll1_t.forward(x_t))

        y_true_t = Variable(torch.from_numpy(y_true))

        lossfn_t = torch.nn.BCELoss(reduction="mean")

        loss_t = lossfn_t(y_t, y_true_t)
        loss_t.backward()

        ll1_twg = ll1_t.weight.grad.numpy()
        ll1_tbg = ll1_t.bias.grad.numpy()

        ll1_tbg = np.reshape(ll1_tbg, newshape=(ll1_tbg.shape[0], 1))

        ll1_t_g = np.hstack((ll1_twg, ll1_tbg))

        self.assertAlmostEqual(
            self.calculate_metric(ll1_t_g, ll1.grad), 0)
        self.assertAlmostEqual(self.calculate_metric(y_t.detach().numpy(), y_pred), 0)


if __name__ == '__main__':
    unittest.main()
