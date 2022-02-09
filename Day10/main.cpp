#include "aoc/helpers.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace {
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view SampleInput(R"(28
33
18
42
31
14
46
20
48
47
24
23
49
45
19
38
39
11
1
32
25
35
8
17
7
9
4
2
34
10
3)");
  constexpr int SR_Part1 = 220;
  constexpr int SR_Part2 = 19208;

  using Input = std::vector<int>;

  const auto LoadInput = [](auto f) {
    Input r;
    std::string_view line;
    while (aoc::getline(f, line)) {
      r.push_back(aoc::stoi(line));      
    }
    return r;
  };

  const auto FindDistribution = [](const auto& input) {
    int previous = 0;
    int jolt_1 = 0;
    int jolt_3 = 0;
    for (const auto& i : input) {
        if (i == previous + 1) { jolt_1++; }
        else if (i == previous + 3) { jolt_3++; }
        previous = i;
    }
    return jolt_1 * (jolt_3 + 1);
  };

  using Memoization = std::unordered_map<int, int64_t>;
  int64_t GetArrangments(size_t start, const Input& input, Memoization& map) {
    if (start == input.size() - 1) { return 1; }

    const auto r = map.find(start);
    if (r != map.end()) {
      return r->second;
    }

    int64_t total = 0;
    for (size_t i = start + 1; i < input.size(); i++) {
      const int64_t range = input[i] - input[start];
      if (range > 3) {
        break;
      }
      total += GetArrangments(i, input, map);
    }

    map.emplace(start, total);

    return total;
  }
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  Input r;
  if (inTest) {
    r = LoadInput(SampleInput);
  } else {
    std::unique_ptr<MappedFileSource>m(new MappedFileSource(argc, argv));
    std::string_view f(m->data(), m->size());
    r = LoadInput(f);
  }

  std::sort(r.begin(), r.end());

  int part1 = FindDistribution(r);

  // Add our built-in adapter at the back
  r.emplace_back(r.back() + 3);
  // And the 0 rated at the front
  r.insert(r.begin(), 0);

  Memoization map;
  int64_t part2 = GetArrangments(0, r, map);

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

