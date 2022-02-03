#include "aoc/helpers.h"
#include <vector>
#include <algorithm>

namespace {
  const std::string SampleInput(R"(1721
  979
  366
  299
  675
  1456)");
  std::stringstream SampleStream(SampleInput);
  constexpr int SR_Part1 = 514579;
  constexpr int SR_Part2 = 241861950;

  const auto LoadInput = [](auto& f) {
    std::vector<int> input;
    aoc::parse_as_integers(f, [&](int i) {
      input.push_back(i);
    });
    return input;
  };

  const auto FindTwoWithSum = [](const std::vector<int>& input, const int target, const size_t start) {
    size_t l = start;
    size_t r = input.size() - 1;

    while (l < r) {
      const int sum = input[l] + input[r];
      if (sum == target) {
        return input[l] * input[r];
      } else if (sum < target) {
        l++;
      } else {
        r--;
      }
    }

    return 0;
  };
};

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

  std::sort(input.begin(), input.end());
  
  int64_t part1 = FindTwoWithSum(input, 2020, 0);

  int64_t part2 = 0;
  for (size_t i = 0; i < input.size() - 2; i++) {
    int base = input[i];
    int rem = 2020 - base;
    int64_t rest = FindTwoWithSum(input, rem, i + 1);
    if (rest) {
      part2 = rest * base;
      break;
    }
  }

  assert(part1);
  assert(part2);

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

