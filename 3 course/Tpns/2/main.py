import nn
import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split


def train(model: nn.Model, loss: nn.LossFunction, opt: nn.Optimizer, X, Y, i_max=10, eps=1e-3):
    for i in range(i_max):

        y_p = model.forward(X)

        loss_val = loss.calc_loss(Y, y_p)
        grad = loss.calc_gradient(Y, y_p)
        model.backward(grad)

        opt.do_step(model)
        model.zerograd()

        if loss_val <= eps:
            break


def test(model: nn.Model, metric: nn.Metric, X, Y):
    y_p = model.forward(X)
    return metric.calc_loss(Y, y_p)


data = pd.read_csv("../1/Laptop_price.csv")
X = data["Storage_Capacity"].to_numpy()
X = np.reshape(X, newshape=(X.shape[0], 1))
y = data["Price"].to_numpy()
y = np.reshape(y, newshape=(y.shape[0], 1))
X_train, X_validate, y_train, y_validate = train_test_split(X, y, train_size=0.8, random_state=42)

X_mean = np.mean(X_train)
X_sigma = np.sqrt(np.var(X_train))
X_train = (X_train - X_mean) / X_sigma

X_validate = (X_validate - X_mean) / X_sigma

model = nn.Model([
    nn.Linear(1, 1),
])
loss_f = nn.MSE()
opt = nn.Optimizer(lr=0.001)

train(model, loss_f, opt, X_train, y_train)
print(test(model, nn.RMSE(), X_validate, y_validate))
