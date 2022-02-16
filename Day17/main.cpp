#include "aoc/helpers.h"

#include <array>
#include <algorithm>
#include <unordered_set>
#include <utility>

template <std::size_t N>
using Point = std::array<int, N>;

namespace std {
  template <std::size_t N>
  struct hash<Point<N>> {
    constexpr std::size_t operator()(Point<N> const& p) const noexcept {
      return [&] <std::size_t ... Is> (std::index_sequence<Is...>) {
        std::size_t h {23};
        (... , (h *= h*31 + p[Is]));
        return h;
      }(std::make_index_sequence<N>{});
    }
  };
}

namespace {
  using Result = std::pair<int, int>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view SampleInput(R"(.#.
..#
###)");
  constexpr int SR_Part1 = 112;
  constexpr int SR_Part2 = 848;

  const auto LoadInput = [](auto f) {
    std::unordered_set<Point<3>> input;
    int y = 0;

    std::string_view line;
    while (aoc::getline(f, line)) {
      int x = 0;
      for (const auto& c : line) {
        if (c == '#') {
          input.insert({x, y, 0});
        }
        x++;
      }
      y++;
    }
    return input;
  };

  template <std::size_t N, std::size_t I = 0>
  void
  _neighbors(Point<N>& orig, std::unordered_set<Point<N>>& ns) {
    if constexpr (I == N) {
      ns.insert(orig);
    } else {
      int loc = orig[I];
      for (int i : {-1, 0, 1}) {
        orig[I] = loc + i;
        _neighbors<N, I + 1>(orig, ns);
      }
      orig[I] = loc;
    }
  }

  template <std::size_t N>
  std::unordered_set<Point<N>>
  get_neighbors(Point<N> const& orig) {
    Point<N> p {orig};
    std::unordered_set<Point<N>> adj;
    _neighbors(p, adj);
    adj.erase(orig);
    return adj;
  }

  template <std::size_t N>
  std::unordered_set<Point<N>>
  step(std::unordered_set<Point<N>> const& locs) {
    std::unordered_set<Point<N>> candidates;
    for (auto& l : locs) {
      for (auto& n : get_neighbors(l)) {
        candidates.insert(std::move(n));
      }
    }
    std::unordered_set<Point<N>> next_locs;
    for (auto const & c : candidates) {
      auto n = get_neighbors(c);
      auto count = std::count_if(
        n.begin(), n.end(),
        [&] (Point<N> const& n) {
          return locs.find(n) != locs.end();
        });
      if ((count == 3) || (locs.find(c) != locs.end() && count == 2)) {
        next_locs.insert(c);
      }
    }
    return next_locs;
  }

  template <std::size_t N>
  int
  simulate(std::unordered_set<Point<N>> const& input, int reps) {
    if (reps == 0) {
      return input.size();
    } else {
      return simulate(step(input), reps - 1);
    }
  }

  template <std::size_t N, std::size_t M>
  Point<N>
  pad (Point<M> const & in) {
    Point<N> out;
    std::fill_n(out.data(), N, 0);
    std::copy_n(in.data(), M, out.data());
    return out;
  }
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  std::unordered_set<Point<3>> r;
  if (inTest) {
    r = LoadInput(SampleInput);
  } else {
    std::unique_ptr<MappedFileSource>m(new MappedFileSource(argc, argv));
    std::string_view f(m->data(), m->size());
    r = LoadInput(f);
  }

  std::unordered_set<Point<4>> r2;
  for (const auto& i : r) {
    r2.insert(pad<4>(i));
  }

  int part1 = simulate(r, 6);
  int part2 = simulate(r2, 6);

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

