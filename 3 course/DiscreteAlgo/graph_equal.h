#pragma once

#include "graph.h"

bool IsSameGraph(const Matrix<int> &graph1, const Matrix<int> &graph2);

template<typename Iterator>
bool Contains(Iterator first, Iterator last, const Matrix<int> &graph) {
  return std::any_of(first,
                     last,
                     [&](const Matrix<int> &graph1) { return IsSameGraph(graph1, graph); });

}