# import nn
import numpy as np

from layers import nn

np.random.seed(100)


def convolution(matrix: np.ndarray, kernel: np.ndarray) -> np.ndarray:
    n, c, xi, yi = matrix.shape
    flat_input = np.reshape(matrix, newshape=(matrix.shape[0], -1))

    newx = xi - kernel.shape[-2] + 1
    newy = yi - kernel.shape[-1] + 1

    mat = get_conv_matrix_for_image(kernel, c, xi, yi)

    out = flat_input @ mat

    out = np.transpose(out, (1, 0, 2))

    out = np.reshape(out, newshape=(matrix.shape[0], kernel.shape[0], newx, newy))

    return out


def get_conv_matrix_for_image(kernel: np.ndarray, c, xi, yi):
    xk = kernel.shape[-2]
    yk = kernel.shape[-1]
    pk = np.pad(kernel, ((0, 0), (0, 0), (0, xi - xk), (0, yi - yk)), mode='constant', constant_values=0)

    flatpk = np.reshape(pk, newshape=(kernel.shape[0], -1))

    newx = xi - xk + 1
    newy = yi - yk + 1
    newsize = newx * newy
    mat = np.zeros(shape=(kernel.shape[0], c * xi * yi, newsize))

    for j in range(kernel.shape[0]):
        for x_t in range(newx):
            for y_t in range(newy):
                mat[j, :, y_t + x_t * newy] = flatpk[j]
                flatpk[j] = np.roll(flatpk[j], 1)
            flatpk[j] = np.roll(flatpk[j], yk - 1)
    return mat


def get_m_with_pad(M: np.ndarray, m: int, n: int) -> np.ndarray:
    ret_mat = np.zeros(shape=(M.shape[0] + 2 * m, M.shape[1] + 2 * n))
    ret_mat[m:M.shape[0] + m, n:M.shape[1] + n] = M
    return ret_mat


class Flatten(nn.Function):

    def __init__(self):
        super().__init__()
        self.last_shape = None

    def forward(self, input: np.ndarray) -> np.ndarray:
        self.input_shape = input.shape
        return input.reshape(input.shape[0], -1)

    def backward(self, out_gradient: np.ndarray) -> np.ndarray:
        return out_gradient.reshape(self.input_shape)


class Conv(nn.Function):
    def __init__(self, input_channels, output_channels, kernel_size, learn_bias=True, pad=0):
        super().__init__()

        self.output_channels = output_channels

        self.m_kernel = kernel_size[0]
        self.n_kernel = kernel_size[1]

        self.learn_bias = learn_bias
        normal_dist = np.random.normal(size=self.m_kernel * self.n_kernel * input_channels * output_channels, )
        self.params = np.reshape(normal_dist, newshape=(output_channels, input_channels, self.m_kernel, self.n_kernel))

        self.grad = np.zeros(shape=(output_channels, input_channels, self.m_kernel, self.n_kernel,))

        self.bias = np.random.normal(size=output_channels)
        self.bias_grad = np.zeros(shape=output_channels)
        self.last_input = None
        self.pad = pad

    def forward(self, input: np.ndarray) -> np.ndarray:
        actual_input = np.pad(input, ((0, 0), (0, 0), (0, self.pad), (0, self.pad)), mode='constant', constant_values=0)
        self.last_input = actual_input

        out = convolution(actual_input, self.params)

        if self.learn_bias:
            out += np.reshape(self.bias, newshape=(1, self.output_channels, 1, 1))

        return out

    def backward(self, out_gradient: np.ndarray):
        dx = np.zeros(shape=self.last_input.shape)

        # n, c, xi, yi = self.last_input.shape

        out_gradient_to_kernel = np.transpose(out_gradient, (1, 0, 2, 3))
        input_t = np.transpose(self.last_input, (1, 0, 2, 3))
        dw = convolution(input_t, out_gradient_to_kernel)
        self.grad += np.transpose(dw, (1, 0, 2, 3))

        self.bias_grad += np.sum(out_gradient, axis=(0, 2, 3))

        with_pad = np.pad(out_gradient, (
            (0, 0), (0, 0), (self.m_kernel - 1, self.m_kernel - 1), (self.n_kernel - 1, self.n_kernel - 1)))
        # with_pad = get_m_with_pad(out_gradient[i], m=self.m_kernel - 1, n=self.n_kernel - 1)

        rotated = np.rot90(np.transpose(self.params, (1, 0, 2, 3)), k=2, axes=(2, 3))

        dx = convolution(with_pad, rotated)

        return dx[:, :, self.pad:dx.shape[2] - self.pad, self.pad:dx.shape[3] - self.pad]


    def zerograd(self):
        self.grad.fill(0)
        self.bias_grad.fill(0)


    def have_grad(self) -> bool:
        return True


class AvgPool2D(nn.Function):
    def __init__(self, pool_size=2, stride=2):
        super().__init__()
        self.cache = None
        self.pool_size = pool_size
        self.stride = stride

    def forward(self, x):
        n, c, h, w = x.shape

        pad_h = (self.pool_size - (h % self.pool_size)) % self.pool_size
        pad_w = (self.pool_size - (w % self.pool_size)) % self.pool_size

        # Pad the input on the bottom and right if necessary
        if pad_h > 0 or pad_w > 0:
            x_padded = np.pad(x, ((0, 0), (0, 0), (0, pad_h), (0, pad_w)), mode='constant', constant_values=0)
            # print(x.shape, x_padded.shape)
        else:
            x_padded = x

        n, c, h_padded, w_padded = x_padded.shape

        x_reshaped = x_padded.reshape(n, c, h_padded // self.pool_size, self.pool_size, w_padded // self.pool_size,
                                      self.pool_size)
        x_strided = x_reshaped.transpose(0, 1, 2, 4, 3, 5).reshape(n, c, h_padded // self.pool_size,
                                                                   w_padded // self.pool_size,
                                                                   self.pool_size * self.pool_size)

        # Perform average pooling
        out = x_strided.mean(axis=4)
        self.cache = (x.shape, x_strided.shape, out.shape, (pad_h, pad_w))  # Include padding in cache for backward
        return out

    def backward(self, dout, **kwargs):
        n, c, h, w = self.cache[0]
        _, _, out_h, out_w, _ = self.cache[1]
        pad_h, pad_w = self.cache[3]

        dx_strided = np.zeros((n, c, out_h, out_w, self.pool_size * self.pool_size))

        # Distribute gradient evenly to each element in the pooling window
        dout_reshaped = dout[:, :, :, :, np.newaxis]
        dx_strided[:] = dout_reshaped / (self.pool_size * self.pool_size)

        # Reshape gradient back to match the input shape
        dx_strided = dx_strided.reshape(n, c, out_h, self.pool_size, out_w, self.pool_size).transpose(0, 1, 2, 4, 3, 5)
        dx = dx_strided.reshape(n, c, h + pad_h, w + pad_w)

        # Remove padding from the gradient if it was added
        if pad_h > 0 or pad_w > 0:
            dx = dx[:, :, :h, :w]

        return dx
