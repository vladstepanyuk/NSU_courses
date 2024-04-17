#include <fstream>
#include <iostream>
#include "graph_brutforce.h"

#include "graph_equal.h"

struct NamedGraph {
  Matrix<int> graph_;
  std::string name_;
};

std::vector<NamedGraph> LoadNamedGraphs(std::istream &is) {
  auto rv = std::vector<NamedGraph> {};

  while (is) {
    auto new_graph = NamedGraph {};
    std::getline(is, new_graph.name_);
    if (new_graph.name_.empty()) {
      break;
    }

    auto graph_str = std::string {};
    std::getline(is, graph_str);
    auto graph_size = static_cast<size_t>(atoi(graph_str.c_str()));
    new_graph.graph_.resize(graph_size);

    for (int i = 0; i < graph_size; ++i) {
      auto line = std::string {};
      std::getline(is, line);
      auto ss = std::stringstream(line);

      for (int j = 0; j < graph_size; ++j) {
        size_t num;
        ss >> num;
        new_graph.graph_[i].emplace_back(num);
      }
    }

    rv.emplace_back(std::move(new_graph));
  }

  return rv;
}

Matrix<int> DeleteIEdge(const Matrix<int> &graph, size_t i) {
  auto hg = Matrix<int>(graph.size() - 1, std::vector<int>(graph.size() - 1, 0));

  for (int j_g = 0, j_hg = 0; j_g < hg.size() + 1; ++j_g) {
    if (j_g == i) {
      continue;
    }

    for (int k_g = 0, k_hg = 0; k_g < hg.size() + 1; ++k_g) {
      if (k_g == i) {
        continue;
      }

      hg[j_hg][k_hg] = graph[j_g][k_g];
      k_hg++;
    }

    ++j_hg;
  }

  return hg;
}

std::vector<std::string> GetStringsEquiv(const Matrix<int> &graph_to_process,
                                         const std::vector<NamedGraph> &graphs_to_compare) {
  auto rv = std::vector<std::string> {};

  for (int i = 0; i < graph_to_process.size(); ++i) {
    auto hg = DeleteIEdge(graph_to_process, i);

    for (auto &[graph, name] : graphs_to_compare) {
      if (IsSameGraph(hg, graph)) {
        rv.emplace_back(name);
        break;
      }
    }

  }

  return rv;
}

std::vector<std::pair<std::string,
                      std::vector<std::string>>> BrutForceWithDeleteVertex(const std::vector<NamedGraph> &graphs_to_delete,
                                                                           const std::vector<NamedGraph> &graphs_to_compare) {
  auto rv = std::vector<std::pair<std::string, std::vector<std::string>>> {};

  for (auto &[graph1, name1] : graphs_to_delete) {
    auto strings = GetStringsEquiv(graph1, graphs_to_compare);

    rv.emplace_back(name1, strings);
  }

  return rv;
}

int main(int argc, char **argv) {
  auto t4f = std::ifstream("../all_4_edges_tournaments.txt");
  auto graphs4 = LoadNamedGraphs(t4f);

  auto t5f = std::ifstream("../all_5_edges_tournaments.txt");
  auto graphs5 = LoadNamedGraphs(t5f);

  auto res = BrutForceWithDeleteVertex(graphs5, graphs4);

  for (auto &[name, strings] : res) {
    std::cout << name << ":\n";
    for (auto &str : strings) {
      std::cout << str << ", ";
    }
    std::cout << '\n';
  }


}

