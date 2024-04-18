import unittest

import numpy as np

import metrics
from sklearn.metrics import f1_score

from sklearn.metrics import auc
from sklearn.metrics import roc_auc_score
from sklearn.metrics import roc_curve


class MyTestCase(unittest.TestCase):
    def test_f1_score(self):
        y_true = np.array([0, 1, 0, 1, 1])
        y_pred = np.array([1, 1, 0, 0, 1])

        self.assertAlmostEqual(metrics.F1Score().calc_loss(y_true, y_pred),
                               f1_score(y_true, y_pred))

    def test_roc_auc(self):
        y_true = np.array([0, 1, 0, 1, 0, 1, 0, 1])
        y_prob = np.random.uniform(0, 1, y_true.shape[0])

        score, tprs, fprs = metrics.RocAuc().calc_loss(y_true, y_prob)
        self.assertAlmostEqual(score, roc_auc_score(y_true, y_prob), delta=1e-4)


if __name__ == '__main__':
    unittest.main()
