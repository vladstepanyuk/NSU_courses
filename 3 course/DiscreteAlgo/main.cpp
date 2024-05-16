#include <fstream>
#include <iostream>
#include <optional>
#include "graph_brutforce.h"
#include <ranges>
#include <span>
#include <algorithm>

#include "graph_equal.h"

namespace rng = std::ranges;
namespace vw = std::views;

enum class VertexType {
  A = 0,
  B = 1,
  C = 2,
  D = 3,
};

std::ostream &operator<<(std::ostream &ostream, const VertexType &type) {
  switch (type) {
    case VertexType::A:ostream << 'a';
      break;
    case VertexType::B:ostream << 'b';
      break;
    case VertexType::C:ostream << 'c';
      break;
    case VertexType::D:ostream << 'd';
      break;
  }
  return ostream;
}

class Potential {
 public:
  Potential() = default;

  void ResetIn() {
    in_--;
  }

  void ResetOut() {
    out_--;
  }

  void AddIn() {
    in_++;
  }

  void AddOut() {
    out_++;
  }

  void ResetTypesIter() {
    possible_types_ = std::nullopt;
    posible_types_i_ = 0;
  };

  std::optional<VertexType> GetType() {
    if (!possible_types_) {
      possible_types_ = GetPossibleTypes();
    }
    if (posible_types_i_ >= possible_types_->size()) {
      return std::nullopt;
    }

    auto t = possible_types_.value()[posible_types_i_++];

    return t;
  }

  [[nodiscard]] std::vector<VertexType> GetPossibleTypes() const {

    if (in_ == 2 && out_ == 0) {
      return {VertexType::A};
    }

    if (in_ == 1 && out_ == 1) {
      return {VertexType::B, VertexType::C};
    }

    if (in_ == 0 && out_ == 1) {
      return {VertexType::B, VertexType::C, VertexType::D};
    }

    if (in_ == 1 && out_ == 0) {
      return {VertexType::A, VertexType::C, VertexType::B};
    }

    if (in_ == 0 && out_ == 2) {
      return {VertexType::D};
    }

    return {VertexType::A, VertexType::B, VertexType::C, VertexType::D};
  }

  [[nodiscard]] bool PotentialError(VertexType t) const {
    auto in = in_;
    auto out = out_;

    switch (t) {
      case VertexType::A:out += 2;
        break;
      case VertexType::D:in += 2;
        break;
      default:out++;
        in++;
    }

    return in > 2 || out > 2;
  }

  [[nodiscard]] bool PotentialError() const {

    return in_ > 2 || out_ > 2;
  }

 private:
  std::vector<size_t> num_blockers_ = {0, 0, 0, 0};

  std::optional<std::vector<VertexType>> possible_types_;
  size_t posible_types_i_ = 0;
  size_t in_ = 0;
  size_t out_ = 0;
};

class EuilerOrientationGenerator {
 public:
  EuilerOrientationGenerator(size_t n) : n_(n) {
    current_ = std::vector<VertexType>(n_, VertexType::A);
    potentials_ = std::vector<Potential>(n_, Potential());

    for (auto i : vw::iota(0ul, n_)) {
      potentials_[i].GetType();
      potentials_[(i + 1) % n_].AddIn();
      potentials_[(i + 3) % n_].AddIn();
    }

  }

  [[nodiscard]] auto GetView() const {
    return vw::all(current_);
  }

  bool Generate() {
    if (done_) {
      return false;
    }

    Brutforce(n_ - 1);

    return !done_;
  }

 private:

  void Brutforce(size_t i) {

    ResetConstraints(i);
    auto t_o = potentials_[i].GetType();

    if (!t_o && i == 0) {
      done_ = true;
      return;
    }

    auto was_iter = false;

    while (!t_o && !done_) {

      if (was_iter) {
        ResetConstraints(i);
      }
      potentials_[i].ResetTypesIter();

      Brutforce(i - 1);
      t_o = potentials_[i].GetType();
      was_iter = true;

    }
    if (done_) {
      return;
    }

    auto t = t_o.value();

    current_[i] = t;
    SetConstraints(i);
    if (CheckError(i)) {
      Brutforce(i);
    }
  }

  bool CheckError(size_t i) {
    bool error1 = i + 1 >= n_ ?
                  potentials_[(i + 1) % n_].PotentialError(current_[(i + 1) % n_]) :
                  potentials_[(i + 1) % n_].PotentialError();
    bool error2 = i + 3 >= n_ ?
                  potentials_[(i + 3) % n_].PotentialError(current_[(i + 3) % n_]) :
                  potentials_[(i + 3) % n_].PotentialError();;

    return error1 || error2;
  }

  void ResetConstraints(size_t i) {
    if (current_[i] == VertexType::A || current_[i] == VertexType::B) {
      potentials_[(i + 1) % n_].ResetIn();
    } else {
      potentials_[(i + 1) % n_].ResetOut();
    }

    if (current_[i] == VertexType::A || current_[i] == VertexType::C) {
      potentials_[(i + 3) % n_].ResetIn();
    } else {
      potentials_[(i + 3) % n_].ResetOut();
    }
  }

  void SetConstraints(size_t i) {
    if (current_[i] == VertexType::A || current_[i] == VertexType::B) {
      potentials_[(i + 1) % n_].AddIn();
    } else {
      potentials_[(i + 1) % n_].AddOut();
    }

    if (current_[i] == VertexType::A || current_[i] == VertexType::C) {
      potentials_[(i + 3) % n_].AddIn();
    } else {
      potentials_[(i + 3) % n_].AddOut();
    }

  }

 private:
  std::vector<Potential> potentials_;
  std::vector<VertexType> current_;
  bool done_ = false;

  size_t n_;
};

int main(int argc, char **argv) {
  auto generator = EuilerOrientationGenerator(8);
  auto res_view = generator.GetView();
  auto i = 0;

  do {

    rng::for_each(res_view, [](VertexType x) {
      std::cout << x;
    });
    std::cout << std::endl;
    ++i;

  } while (generator.Generate());

  std::cout << i << std::endl;
}

