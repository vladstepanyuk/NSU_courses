import numpy as np

np.random.seed(100)

eps = 1e-6


class Function:
    def __init__(self):
        self.params = None
        self.grad = None
        self.bias = None
        self.bias_grad = None
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
    def __init__(self, input_n, output_n, learn_bias=True):
        super().__init__()

        input_n_actual = input_n
        if learn_bias:
            input_n_actual += 1

        self.learn_bias = learn_bias
        self.grad = np.zeros(shape=(output_n, input_n_actual))
        self.last_input = None
        normal_dist = np.random.normal(size=input_n_actual * output_n, )
        self.params = np.reshape(normal_dist, newshape=(output_n, input_n_actual))

    def forward(self, input: np.ndarray):
        input_actual = input
        if self.learn_bias:
            bias_col = np.ones(shape=(input.shape[0], 1))
            input_actual = np.hstack((input, bias_col))
        self.last_input = input_actual
        out = input_actual @ self.params.T
        return out

    def backward(self, out_gradient: np.ndarray):
        self.grad += (out_gradient.T @ self.last_input)

        next_grad = out_gradient @ self.params

        if self.learn_bias:
            next_grad = next_grad[:, :-1]

        return next_grad

    def zerograd(self):
        self.grad.fill(0)

    def have_grad(self) -> bool:
        return True

    def get_params(self):
        if not self.learn_bias:
            return self.params
        return self.params[:, :-1]

    def get_bias(self):
        if not self.learn_bias:
            raise "No bias"
        return self.params[:, -1].T


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


class Sigmoid(Function):
    def __init__(self):
        super().__init__()
        self.last_input = 0

    @staticmethod
    def calc(x):
        return 1 / (1 + np.exp(-x))

    def forward(self, input: np.ndarray) -> np.ndarray:
        self.last_input = input

        return Sigmoid.calc(input)

    def backward(self, out_gradient: np.ndarray) -> np.ndarray:
        sig = Sigmoid.calc(self.last_input)
        return out_gradient * sig * (1 - sig)

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
        return np.sum(err * err) / err.shape[0]

    def calc_gradient(self, y_true, y_pred):
        err = y_true - y_pred
        return - 2 * err


class CrossEntropy(LossFunction):

    def calc_loss(self, y_true, y_pred):
        hv = y_true * np.log(y_pred + eps) + (1 - y_true) * np.log(1 - y_pred + eps)

        return -(np.sum(hv)) / y_true.shape[0]

    def calc_gradient(self, y_true, y_pred):
        err = y_pred - y_true
        return err / (y_pred * (1 - y_pred) * y_true.shape[0] + eps)


class Optimizer:
    def __init__(self, lr: float):
        self.lr = lr

    def do_step_layer(self, layer: Function):
        layer.params -= self.lr * layer.grad
        # print(np.sum(layer.grad * layer.grad))
        if not (layer.bias is None):
            layer.bias -= self.lr * layer.bias_grad

    def do_step(self, model: Model):
        for l in model.layers:
            if l.have_grad():
                self.do_step_layer(l)


class Softmax(Function):
    def __init__(self):
        super().__init__()
        self.output_data = None

    def forward(self, input_data: np.ndarray) -> np.ndarray:
        exp_values = np.exp(input_data - np.max(input_data, axis=-1, keepdims=True))
        probabilities = exp_values / np.sum(exp_values, axis=-1, keepdims=True)
        self.output_data = probabilities
        return self.output_data

    def backward(self, d_output: np.ndarray, **kwargs) -> np.ndarray:
        return d_output


class CategoricalCrossEntropy(LossFunction):

    def __init__(self, n_classes=2):
        super().__init__()
        self.n_classes = n_classes

    def calc_loss(self, y_true: np.ndarray, y_pred: np.ndarray) -> float:
        epsilon = 1e-15
        clipped_y_pred = np.clip(y_pred, epsilon, 1 - epsilon)
        return -np.sum(y_true * np.log(clipped_y_pred)) / y_pred.shape[0]

    def calc_gradient(self, y_true: np.ndarray, y_pred: np.ndarray) -> np.ndarray:
        epsilon = 1e-15
        clipped_y_pred = np.clip(y_pred, epsilon, 1 - epsilon)
        return -(y_true - clipped_y_pred) / y_pred.shape[0]
