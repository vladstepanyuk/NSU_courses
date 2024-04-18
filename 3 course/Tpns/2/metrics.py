import numpy as np

eps = 1e-5


def calc_confusion_matrix(y_true, y_pred):
    mask = y_true == 1
    tp = np.sum(y_pred[mask] == 1)
    fn = np.sum(mask) - tp

    fp = np.sum(y_pred[np.logical_not(mask)] == 1)
    tn = np.sum(np.logical_not(mask)) - fp

    return np.array([[tp, fn], [fp, tn]])


def perf_metrics(y_actual, y_hat, threshold):
    mask = y_hat >= threshold
    y_predict = np.zeros(shape=y_actual.shape)
    y_predict[mask] = 1

    conf_matrix = calc_confusion_matrix(y_actual, y_predict)

    tp = conf_matrix[0][0]
    fn = conf_matrix[0][1]
    fp = conf_matrix[1][0]
    tn = conf_matrix[1][1]

    tpr = tp / (tp + fn + eps)
    fpr = fp / (tn + fp + eps)

    return fpr, tpr


class Metric:
    def __init__(self):
        """init"""

    def calc_loss(self, y_true: np.ndarray, y_pred: np.ndarray):
        """loss calculation"""


class RMSE(Metric):
    def calc_loss(self, y_true: np.ndarray, y_pred: np.ndarray):
        err = y_true - y_pred
        return np.sqrt(np.sum(err * err)) / len(err)


class F1Score(Metric):

    def calc_loss(self, y_true: np.ndarray, y_pred: np.ndarray):
        conf_matrix = calc_confusion_matrix(y_true, y_pred)

        res = 2 * conf_matrix[0][0] / (2 * conf_matrix[0][0] + conf_matrix[1][0] + conf_matrix[0][1])

        return res


class RocAuc(Metric):

    def calc_loss(self, y_true: np.ndarray, y_prob: np.ndarray):
        thresholds = np.sort(np.unique(y_prob))
        points_fpr = []
        points_tpr = []

        for t in thresholds[::-1]:
            fpr, tpr = perf_metrics(y_true, y_prob, t)
            points_fpr.append(fpr)
            points_tpr.append(tpr)

        auc = np.sum(np.trapz(np.array(points_tpr), np.array(points_fpr)))

        return auc, points_tpr, points_fpr
