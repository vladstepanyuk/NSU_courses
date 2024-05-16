import unittest

import numpy as np

import torch
from torch.autograd import Variable

import conv_nn as nn

from conv_nn import convolution


def calculate_metric(X1: np.ndarray, X2: np.ndarray):
    err = (X1 - X2)
    return np.sum(err * err) / X1.size


class TestConv(unittest.TestCase):

    def setUp(self):
        self.image = np.array([[[[1, 2, 3], [4, 5, 6], [7, 8, 9]], [[1, 2, 3], [4, 5, 6], [7, 8, 9]]]], dtype='float32')
        self.layer = nn.Conv(2, 3, (2, 2))
        self.image_t = Variable(torch.from_numpy(self.image[0]))
        kernels = Variable(torch.from_numpy(self.layer.params))
        bias = Variable(torch.from_numpy(self.layer.bias))

        self.layer_t = torch.nn.Conv2d(2, 3, 2)

        # print(self.layer_t.weight)

        with torch.no_grad():
            self.layer_t.weight.copy_(kernels)
            self.layer_t.bias.copy_(bias)

    def test_backward_conv(self):
        E = np.ones(self.image.shape[1:], dtype='float32')

        E_t = Variable(torch.from_numpy(E), requires_grad=True)

        in_t = torch.mul(E_t, self.image_t)
        out = self.layer_t.forward(in_t)
        self.layer.forward(self.image)

        grad = np.array([[[[1, 1],
                           [1, 1]],
                          [[1, 2],
                           [3, 4]],
                          [[1, 2],
                           [3, 4]]
                          ]])
        grad_t = Variable(torch.from_numpy(grad[0]))

        out.backward(grad_t)
        grad_out = self.layer.backward(grad)

        # print(self.layer.grad)

        # print(self.layer.grad)
        # print(self.layer_t.weight.grad)

        self.assertAlmostEqual(
            calculate_metric(self.layer_t.weight.grad.detach().numpy(), self.layer.grad), 0)

        self.assertAlmostEqual(
            calculate_metric(self.layer_t.bias.grad.detach().numpy(), self.layer.bias_grad), 0)

        print(grad_out * self.image)
        print(E_t.grad)

        self.assertAlmostEqual(
            calculate_metric(E_t.grad.detach().numpy(), self.image[0] * grad_out[0]), 0)

        self.layer_t.zero_grad()
        self.layer.zerograd()

    def test_forward_conv(self):
        out_t = self.layer_t.forward(self.image_t)
        out = self.layer.forward(self.image)
        print(out)
        print(out_t)
        self.assertAlmostEqual(
            calculate_metric(out_t.detach().numpy(), out[0]), 0)


class TestPool(unittest.TestCase):

    def setUp(self):
        self.image = np.array([[[[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]]]], dtype='float32')
        self.layer = nn.AvgPool2D(2, 2)
        self.image_t = Variable(torch.from_numpy(self.image))
        self.layer_t = torch.nn.AvgPool2d(2, 2)

    def test_backward_conv(self):
        E = np.ones(self.image.shape)
        E_t = Variable(torch.from_numpy(E), requires_grad=True)

        in_t = torch.mul(E_t, self.image_t)
        out = self.layer_t.forward(in_t)
        self.layer.forward(self.image)

        grad = np.array([[[[3.5, 5.5],
                           [11.5, 13.5]]]])
        grad_t = Variable(torch.from_numpy(grad))

        out.backward(grad_t)
        grad_out = self.layer.backward(grad)

        self.assertAlmostEqual(
            calculate_metric(E_t.grad.detach().numpy(), self.image * grad_out), 0)

        self.layer_t.zero_grad()
        self.layer.zerograd()

    def test_forward_conv(self):
        out_t = self.layer_t.forward(self.image_t)
        out = self.layer.forward(self.image)
        # print(out)
        # print(out_t)
        self.assertAlmostEqual(
            calculate_metric(out_t.detach().numpy(), out), 0)


if __name__ == '__main__':
    unittest.main()
