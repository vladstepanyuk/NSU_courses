import pandas as pd
import numpy as np

from sklearn.base import TransformerMixin
from sklearn.preprocessing import StandardScaler

E = 1e-10


def continuous_feature_to_category(data: pd.DataFrame, continuous_features):
    data_cp = data.copy()

    for column in continuous_features:
        data_cp[column] = pd.cut(data_cp[column], bins=1 + int(np.log2(data.shape[0])))

    return data_cp


def calc_entropy(column: pd.Series):
    p = column.value_counts(normalize=True)
    return p.apply(lambda x: - x * np.log2(x + E)).aggregate("sum")


def calculate_gain_ratio(column: pd.DataFrame, target: pd.Series):
    p_feature = column.value_counts(normalize=True)
    p_target = target.value_counts(normalize=True)
    split_info = p_feature.apply(lambda x: - x * np.log2(x + E)).aggregate("sum")
    cur_entropy = p_target.apply(lambda x: - x * np.log2(x + E)).aggregate("sum")
    weighted_child_entropy = 0
    for key in p_feature.keys():
        weighted_child_entropy += p_feature[key] * calc_entropy(target[column == key])

    return (cur_entropy - weighted_child_entropy) / split_info

