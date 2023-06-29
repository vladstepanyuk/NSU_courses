import seaborn as sns
import numpy as np
import matplotlib.pyplot as plt
import my_functions as func
import pandas as pd

uniform_distribution_file = open("U.txt", "r")
data = np.array(uniform_distribution_file.read().replace('\n', ' ').split(' ')).astype("float64")
sns.displot(data, kind="hist", stat="density", bins=int(np.log2(data.size)) + 1, binrange=(0, 1))
plt.show()