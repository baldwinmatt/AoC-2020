#include "aoc/helpers.h"

#include <vector>

namespace {
  using IntVector = std::vector<int>;

  const std::string SampleInput(R"()");
  std::stringstream SampleStream(SampleInput);
  constexpr int SR_Part1 = 0;
  constexpr int SR_Part2 = 0;

  const auto LoadInput = [](auto& f) {
    std::vector<int> input;
    aoc::parse_as_integers(f, [&](int i) {
      input.push_back(i);
    });
    return input;
  };
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  std::vector<int>input;
  if (inTest) {
    input = LoadInput(SampleStream);
  } else {
    auto f = aoc::open_argv_1(argc, argv);
    input = LoadInput(f);
  }

  int part1 = 0;
  int part2 = 0;

  assert(part1);
  assert(part2);

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

