#include "aoc/helpers.h"

#include <array>
#include <numeric>

namespace {
  using Result = std::pair<int, int>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  using AnswerSet = std::array<int, 26>;

  constexpr std::string_view SampleInput(R"(abc

a
b
c

ab
ac

a
a
a
a

b)");
  constexpr int SR_Part1 = 11;
  constexpr int SR_Part2 = 6;

  const auto LoadInput = [](auto f) {
    Result r{0, 0};
    std::string_view line;

    std::pair<AnswerSet, int> group;
    int sz = 0;

    while (aoc::getline(f, line, "\r\n", true)) {
      if (line.empty()) {
        r.first += group.second;
        r.second += std::accumulate(group.first.cbegin(), group.first.cend(), 0, [&sz](const auto& v, const auto& n) { return v + (n == sz); });
        group.second = 0;
        group.first = AnswerSet();
        sz = 0;
        continue;
      }

      for (const auto &c : line) {
        if (c < 'a' || c > 'z') {
          continue;
        }
        int v = c - 'a';
        group.second += (group.first[v] == 0);
        group.first[v]++;
      }
      sz ++;
    }
    r.first += group.second;
    r.second += std::accumulate(group.first.cbegin(), group.first.cend(), 0, [&sz](const auto& v, const auto& n) { return v + (n == sz); });

    return r;
  };
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  Result r;
  if (inTest) {
    r = LoadInput(SampleInput);
  } else {
    std::unique_ptr<MappedFileSource>m(new MappedFileSource(argc, argv));
    std::string_view f(m->data(), m->size());
    r = LoadInput(f);
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

