import unittest

import numpy as np

import metrics
from sklearn.metrics import f1_score

from sklearn.metrics import roc_auc_score


def calculate_metric(X1: np.ndarray, X2: np.ndarray):
    err = (X1 - X2)
    return np.sum(err * err)


class MyTestCase(unittest.TestCase):

    def test_f1_score(self):
        y_true = np.array([0, 1, 0, 1, 1])
        y_pred = np.array([1, 1, 0, 0, 1])

        self.assertAlmostEqual(metrics.F1Score().calc_metric(y_true, y_pred),
                               f1_score(y_true, y_pred))

    def test_f1_score_multiclass(self):
        y_true = np.array([0, 1, 2, 1, 1])
        y_pred = np.array([1, 1, 2, 0, 1])

        self.assertAlmostEqual(metrics.F1Score().calc_metric(y_true, y_pred),
                               f1_score(y_true, y_pred, average="micro"))

    def test_roc_auc(self):
        y_true = np.array([0, 1, 0, 1, 0, 1, 0, 1])
        y_prob = np.random.uniform(0, 1, y_true.shape[0])

        score, tprs, fprs = metrics.RocAuc().calc_metric(y_true, y_prob)
        self.assertAlmostEqual(score, roc_auc_score(y_true, y_prob), delta=1e-4)

    def test_roc_auc_multiclass(self):
        y_true = np.zeros(shape=(100,))
        y_true[0] = 0
        y_true[1] = 1
        y_true[2] = 2
        y_prob = np.random.uniform(0, 0.5, (y_true.shape[0], 3))
        y_prob[:, 2] = 1 - y_prob[:, 0] - y_prob[:, 1]

        score, _, _ = metrics.RocAuc().calc_metric(y_true, y_prob)

        score_sklearn = roc_auc_score(y_true, y_prob, average=None, multi_class='ovr')

        self.assertAlmostEqual(
            calculate_metric(score, score_sklearn), 0.0,
            delta=1e-4)


if __name__ == '__main__':
    unittest.main()
