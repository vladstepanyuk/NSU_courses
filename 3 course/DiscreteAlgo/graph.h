#pragma once

#include <vector>
#include <iterator>
#include <sstream>

template<typename T>
using Matrix = std::vector<std::vector<T>>;

template<typename T>
std::ostream &operator<<(std::ostream &ostream, const Matrix<T> &graph) {
  std::stringstream str_stream;
  for (const auto &i : graph) {
    std::copy(i.begin(), i.end(), std::ostream_iterator<T>(str_stream, " "));
    str_stream << '\n';
  }

  ostream << str_stream.str();

  return ostream;
}