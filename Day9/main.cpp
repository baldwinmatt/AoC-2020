#include "aoc/helpers.h"

#include <vector>

namespace {
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view SampleInput(R"(35
20
15
25
47
40
62
55
65
95
102
117
150
182
127
219
299
277
309
576)");
  constexpr int SR_Part1 = 127;
  constexpr int SR_Part2 = 62;

  using Result = std::pair<int, int>;

  const auto FindTwoWithSum = [](const std::vector<int64_t>& input, const int64_t target, const size_t start) {
    int64_t ret = 0;
    for (size_t base = start; base < input.size() - 1; base++) {
      const int64_t need = target - input[base];

      for (size_t i = start + 1; i < input.size(); i++) {
        if (input[i] == need) {
          return need * input[base];
        }
      }
    }

    return ret;
  };

  const auto FindContiguousMinMax = [](const std::vector<int64_t>& input, const int64_t target) {
    int64_t ret = 0;
    size_t end = input.size() - 2;
    for (size_t i = 0; i < end; i++) {
      int64_t window_size = input[i];
      size_t j = i + 1;
      int64_t min = window_size;
      int64_t max = window_size;
      while (window_size < target && j < end) {
        min = std::min(min, input[j]);
        max = std::max(max, input[j]);
        window_size += input[j];
        j++;
      }
      if (window_size == target) {
        return min + max;
      }
    }
    return ret;
  };

  const auto LoadInput = [](auto f, size_t prolog_size) {
    Result r{0, 0};
    std::string_view line;
    std::vector<int64_t> prolog;
    while (aoc::getline(f, line)) {
      int64_t num = aoc::stoi(line);
      if (prolog.size() < prolog_size) {
        prolog.push_back(num);
        continue;
      }

      if (!FindTwoWithSum(prolog, num, prolog.size() - prolog_size)) {
        r.first = num;

        r.second = FindContiguousMinMax(prolog, num);

        return r;
      }
      prolog.push_back(num);
    }
    return r;
  };
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  Result r;
  if (inTest) {
    r = LoadInput(SampleInput, 5);
  } else {
    std::unique_ptr<MappedFileSource>m(new MappedFileSource(argc, argv));
    std::string_view f(m->data(), m->size());
    r = LoadInput(f, 25);
  }

  int part1 = 0;
  int part2 = 0;

  std::tie(part1, part2) = r;

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

