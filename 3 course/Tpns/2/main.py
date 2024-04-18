from sklearn.preprocessing import OneHotEncoder
import nn
import metrics
from sklearn.model_selection import train_test_split
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt


def train(model: nn.Model, loss: nn.LossFunction, opt: nn.Optimizer, X, Y, i_max=100, eps=1e-3):
    for i in range(i_max):

        y_p = model.forward(X)

        loss_val = loss.calc_loss(Y, y_p)
        grad = loss.calc_gradient(Y, y_p)
        model.backward(grad)

        opt.do_step(model)
        model.zerograd()

        print(loss_val)
        if loss_val <= eps:
            break


# %%
def test(model: nn.Model, metric: metrics.Metric, X, Y, treshold=0.5):
    y_p = model.forward(X)
    mask = y_p >= treshold
    y_p[mask] = 1
    y_p[np.logical_not(mask)] = 0
    return metric.calc_loss(Y, y_p)


df = pd.read_csv('./mushrooms.csv')

X = df.drop('class', axis=1)
y = pd.DataFrame(df['class'])

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.33, random_state=34)

columns_to_drop = ["gill-attachment",
                   "gill-spacing",
                   "ring-number",
                   "veil-color",
                   "veil-type", ]

X_train = X_train.drop(columns=columns_to_drop)

encoder = OneHotEncoder()

encoder.fit(X_train)
X_act_train = encoder.transform(X_train)

y_train[y_train == 'p'] = 1
y_train[y_train == 'e'] = 0

y_train = y_train.astype("float32")

features = [82, 98, 64, 77, 7, 76, 54, 89, 25, 22, 88, 59, 51, 49, 27]

X_train_numpy = X_act_train.toarray()

X_train_numpy = X_train_numpy[:, features]
y_train_numpy = y_train.to_numpy()
# %%
# print(X_train_numpy.shape)

model = nn.Model([
    nn.Linear(15, 15),
    nn.Relu(),
    nn.Linear(15, 1),
    nn.Sigmoid(),
])

loss_f = nn.CrossEntropy()
opt = nn.Optimizer(lr=0.1)
# %%
train(model, loss_f, opt, X_train_numpy, y_train_numpy)

X_act_test = X_test.drop(columns=columns_to_drop)
X_act_test = encoder.transform(X_act_test)
X_test_numpy = X_act_test.toarray()
X_test_numpy = X_test_numpy[:, features]

y_test[y_test == 'p'] = 1
y_test[y_test == 'e'] = 0

y_test = y_test.astype("float32")

metric = metrics.F1Score()
print(test(model, metric, X_test_numpy, y_test.to_numpy()))
