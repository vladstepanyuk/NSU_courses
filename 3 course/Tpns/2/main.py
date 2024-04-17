import nn
import numpy as np

if __name__ == '__main__':
    model = nn.Model([
        nn.Linear(3, 4),
    ])

    opt = nn.Optimizer(0.01)

    x = np.array([2, 1, 2], dtype="float64").reshape((3, 1))
    y_true = np.array([1, 2, 3, 2], dtype="float64").reshape(4, 1)
    loss_f = nn.MSE()

    for i in range(30000):
        y_p = model.forward(x)

        loss = loss_f.calc_loss(y_true, y_p)
        grad = loss_f.calc_gradient(y_true, y_p)
        model.backward(grad)
        opt.do_step(model)
        model.zerograd()
        print(loss)

    print(model.layers[0].params)
