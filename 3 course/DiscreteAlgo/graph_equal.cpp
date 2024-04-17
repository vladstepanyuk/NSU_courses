//
// Created by 1 on 26.02.2024.
//

#include "graph_equal.h"



bool IsSameGraph(const Matrix<int> &graph1, const Matrix<int> &graph2) {
  if (graph1.size() != graph2.size()) {
    return false;
  }

  std::vector<size_t> perm(graph1.size());

  for (size_t i = 0; i < perm.size(); ++i) {
    perm[i] = i;
  }

  bool flag;

  while (true) {
    flag = true;
    for (int i = 0; i < perm.size() && flag; ++i) {
      for (int j = 0; j < perm.size() && flag; ++j) {
        if (graph1[perm[i]][perm[j]] != graph2[i][j]) {
          flag = false;
        }
      }

    }

    if (flag || !std::next_permutation(perm.begin(), perm.end())) {
      break;
    }
  }

  return flag;
}