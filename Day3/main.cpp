#include "aoc/helpers.h"

#include <vector>

namespace {
  using Row = std::vector<bool>;
  using Grid = std::vector<Row>;

  const auto IsTree = [](const Grid& g, size_t x, size_t y) {
    assert(y < g.size());
    const auto& r = g[y];
    
    x = x % r.size();
    return r[x];
  };

  const std::string SampleInput(R"(..##.......
#...#...#..
.#....#..#.
..#.#...#.#
.#...##..#.
..#.##.....
.#.#.#....#
.#........#
#.##...#...
#...##....#
.#..#...#.#)");
  std::stringstream SampleStream(SampleInput);
  constexpr int SR_Part1 = 7;
  constexpr int SR_Part2 = 336;

  const auto LoadInput = [](auto& f) {
    std::string row;
    Grid input;
    size_t w = 0;

    while (aoc::getline(f, row)) {
      Row r;
      for (const auto& c: row) {
        switch (c) {
          case '.':
            r.push_back(false);
            break;
          case '#':
            r.push_back(true);
            break;
          default:
            throw std::runtime_error("Not a space or tree");
        }
      }

      if (w && r.size() != w) {
        throw std::runtime_error("Inconsistent input");
      } else if (!w) {
        w = r.size();
      }

      input.push_back(r);
    }

    return input;
  };
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  Grid input;
  if (inTest) {
    input = LoadInput(SampleStream);
  } else {
    auto f = aoc::open_argv_1(argc, argv);
    input = LoadInput(f);
  }

  int64_t part1 = 0;
  int64_t part2 = 1;

  std::vector<std::pair<size_t, size_t>> slopes = {
    { 3, 1 },
    { 1, 1 },
    { 5, 1 },
    { 7, 1 },
    { 1, 2 },
  };
  for (const auto& slope : slopes) {
    int count = 0;
    size_t x = 0;
    size_t y = 0;
    while (y < input.size()) {
      count += IsTree(input, x, y);
      x += slope.first;
      y += slope.second;
    }

    DEBUG_PRINT("Slope: { " << slope.first << ", " << slope.second << " }: " << count);

    if (!part1) { part1 = count; }
    part2 *= count;
  }

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

