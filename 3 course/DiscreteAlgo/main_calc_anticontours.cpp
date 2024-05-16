#include <iostream>
#include <vector>
#include <numeric>
#include <iterator>
#include <ranges>
#include <fstream>

namespace vw = std::views;

template<typename T>
using Matrix = std::vector<std::vector<T>>;

template<typename T>
Matrix<T> MakeMatrix(size_t n, size_t m, T std_val = T {}) {
  Matrix<T> res(n, std::vector<T>(m, std_val));

  return res;
}

Matrix<int> DecodeMatrix(std::string_view s) {
  auto n = s.size();
  auto mat = MakeMatrix<int>(n, n);

  for (auto i : std::views::iota(0ul, n)) {
    switch (s[i]) {
      case 'a':mat[i][(i + 1) % n] = 1;
        mat[(i + 1) % n][i] = -1;
        mat[i][(i + 3) % n] = 1;
        mat[(i + 3) % n][i] = -1;
        break;
      case 'b':mat[i][(i + 1) % n] = 1;
        mat[(i + 1) % n][i] = -1;
        mat[(i + 3) % n][i] = 1;
        mat[i][(i + 3) % n] = -1;
        break;
      case 'c':mat[(i + 1) % n][i] = 1;
        mat[i][(i + 1) % n] = -1;
        mat[i][(i + 3) % n] = 1;
        mat[(i + 3) % n][i] = -1;
        break;
      case 'd':mat[(i + 1) % n][i] = 1;
        mat[i][(i + 1) % n] = -1;
        mat[(i + 3) % n][i] = 1;
        mat[i][(i + 3) % n] = -1;
        break;
      default:throw std::runtime_error("unexpected code char");
    }
  }

  return mat;
}

std::vector<std::pair<std::string, Matrix<int>>> ReadGraphs(std::istream &is, size_t graph_num) {
  auto rv = std::vector<std::pair<std::string, Matrix<int>>>();
  rv.reserve(graph_num);

  std::string code;
  for (auto _ : vw::iota(0ul, graph_num)) {
    is >> code;

    auto mat = DecodeMatrix(code);

    rv.emplace_back(std::move(code), std::move(mat));
    code.clear();
  }

  return rv;
}

void AntiContourCrawler(const Matrix<int> &graph,
                        size_t i,
                        int last,
                        std::vector<bool> &was_there,
                        std::vector<int> &res) {
  was_there[i] = true;

  for (size_t j = 0; j < graph[i].size(); ++j) {
    if (graph[i][j] != 0 && !was_there[j] && graph[i][j] != last) {
      res.emplace_back(j);
      AntiContourCrawler(graph, j, graph[i][j], was_there, res);
      return;
    }
  }
}

std::vector<std::vector<int>> ProcessGraph(const Matrix<int> &graph) {
  auto res = std::vector<std::vector<int>>();
  auto was_there = std::vector<bool>(graph.size(), false);
  for (int i = 0; i < graph.size(); ++i) {
    if (!was_there[i]) {
      res.emplace_back();
      res.back().emplace_back(i);

      AntiContourCrawler(graph, i, 0, was_there, res.back());
    }
  }
  return res;
}

int GetEdgeVal(int i, int j, size_t graph_size) {

  if ((i + 1) % graph_size == j) {
    return 1;
  }

  if ((i + 3) % graph_size == j) {
    return 3;
  }

  if ((j + 1) % graph_size == i) {
    return -1;
  }

  if ((j + 3) % graph_size == i) {
    return -3;
  }

  return 1488;
}

int main(int argc, char **argv) {

  size_t graph_num;

  std::cin >> graph_num;

  auto graphs = ReadGraphs(std::cin, graph_num);

  for (auto &&[code, graph] : graphs) {
    std::cout << code << std::endl;
    auto graph_copy = graph;

    auto res = ProcessGraph(graph_copy);

    for (auto anticontour : res) {
      for (size_t j = 0; j < anticontour.size(); ++j) {
        graph_copy[anticontour[j]][anticontour[(j + 1) % anticontour.size()]] = 0;
        graph_copy[anticontour[(j + 1) % anticontour.size()]][anticontour[j]] = 0;
      }
    }
    auto res1 = ProcessGraph(graph_copy);

    for (auto anticontour : res) {
      std::copy(anticontour.begin(), anticontour.end(), std::ostream_iterator<int>(std::cout, " "));
      std::cout << std::endl;
      for (size_t j = 0; j < anticontour.size(); ++j) {
        std::cout << GetEdgeVal(anticontour[j], anticontour[(j + 1) % anticontour.size()], graph.size()) << ' ';
      }
      std::cout << std::endl;
    }
    for (auto anticontour : res1) {
      std::copy(anticontour.begin(), anticontour.end(), std::ostream_iterator<int>(std::cout, " "));
      std::cout << std::endl;
      for (size_t j = 0; j < anticontour.size(); ++j) {
        std::cout << GetEdgeVal(anticontour[j], anticontour[(j + 1) % anticontour.size()], graph.size()) << ' ';
      }
      std::cout << std::endl;
    }

    std::cout << std::endl;
  }

}