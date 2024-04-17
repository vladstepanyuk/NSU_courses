//
// Created by 1 on 26.02.2024.
//

#include <complex>
#include <fstream>
#include "graph_brutforce.h"
#include "graph_equal.h"

void ToNextTournament(Matrix<int> &graph) {
  auto to_next = 1;

  for (int i = 0; i < graph.size() && to_next == 1; ++i) {

    for (int j = i + 1; j < graph[i].size() && to_next == 1; ++j) {
      auto new_num = graph[i][j] + to_next;
      graph[i][j] = new_num % 2;
      to_next = new_num / 2;

      graph[j][i] = 1 - graph[i][j];
    }
  }
}

void BrutForce5Tournaments() {
  auto graph = Matrix<int> {{false, false, false, false, false},
                            {true, false, false, false, false},
                            {true, true, false, false, false},
                            {true, true, true, false, false},
                            {true, true, true, true, false}};

  auto max_state = static_cast<size_t>(std::pow(2, 10));
  auto all_graphs = std::vector<Matrix<int>>();
  all_graphs.reserve(12);

  for (size_t i = 0; i < max_state; ++i) {
    if (!Contains(all_graphs.begin(), all_graphs.end(), graph)) {
      all_graphs.emplace_back(graph);
    }

    ToNextTournament(graph);
  }

  std::ofstream out("all_5_edges_tournaments.txt");

  for (auto &graph1 : all_graphs) {
    out << graph1 << '\n' << '\n';
  }
}


void BrutForce4Tournaments() {
  auto graph = Matrix<int> {{false, false, false, false},
                            {true, false, false, false},
                            {true, true, false, false},
                            {true, true, true, false}};

  auto max_state = static_cast<size_t>(std::pow(2, 6));
  auto all_graphs = std::vector<Matrix<int>>();
  all_graphs.reserve(4);

  for (size_t i = 0; i < max_state; ++i) {
    if (!Contains(all_graphs.begin(), all_graphs.end(), graph)) {
      all_graphs.emplace_back(graph);
    }

    ToNextTournament(graph);
  }

  std::ofstream out("all_4_edges_tournaments.txt");

  for (auto &graph1 : all_graphs) {
    out << graph1 << '\n' << '\n';
  }
}
