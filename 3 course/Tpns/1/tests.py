import unittest

import numpy as np
import pandas as pd
import scipy.stats

import functions as f


class Tests(unittest.TestCase):
    def test_gain_ratio(self):
        test_matrix = pd.DataFrame(data=[
            [1, 1],
            [1, 1],
            [1, 0],
            [0, 0],
            [2, 1]
        ])

        gain_ratio = f.calculate_gain_ratio(test_matrix[0], test_matrix[1])

        split_info = - 3 / 5 * np.log2(3 / 5) - 1 / 5 * np.log2(1 / 5) - 1 / 5 * np.log2(
            1 / 5)  # split info for test_matrix[0]
        cur_entropy = - 3 / 5 * np.log2(3 / 5) - 2 / 5 * np.log2(2 / 5)  # entropy for test_matrix[1]

        weighted_child_entropy = 3 / 5 * (- 2 / 3 * np.log2(2 / 3) - 1 / 3 * np.log2(1 / 3)) + 1 / 5 * 0 + 1 / 5 * 0

        true_gain_ratio = (cur_entropy - weighted_child_entropy) / split_info

        self.assertAlmostEqual(gain_ratio, true_gain_ratio)

    def test_entropy(self):
        test_column1 = pd.Series(data=[1, 1, 1, 1])
        self.assertAlmostEqual(scipy.stats.entropy(pk=test_column1.value_counts(normalize=True), base=2),
                               f.calc_entropy(test_column1))

        test_column1 = pd.Series(data=[1, 1, 0, 0])
        self.assertAlmostEqual(scipy.stats.entropy(pk=test_column1.value_counts(normalize=True), base=2),
                               f.calc_entropy(test_column1))

        test_column1 = pd.Series(data=[1, 2, 3, 4])
        self.assertAlmostEqual(scipy.stats.entropy(pk=test_column1.value_counts(normalize=True), base=2),
                               f.calc_entropy(test_column1))
        




if __name__ == '__main__':
    unittest.main()
