#include "aoc/helpers.h"

#include <memory>
#include <numeric>
#include <vector>
#include <string>
#include <string_view>

namespace {
  using Password = std::tuple<int, int, char, std::string>;

  const std::string SampleInput(R"(1-3 a: abcde
1-3 b: cdefg
2-9 c: ccccccccc)");
  std::stringstream SampleStream(SampleInput);
  constexpr int SR_Part1 = 2;
  constexpr int SR_Part2 = 1;

  const auto PasswordIsValid = [](const auto& p) {
    int fmin, fmax;
    char c;
    std::string pwd;
    std::tie(fmin, fmax, c, pwd) = p;

    const int count = std::accumulate(pwd.cbegin(), pwd.cend(), 0,
      [&](const auto& sum, const auto& ch) {
        return sum + (ch == c);
      }
    );
    return count >= fmin && count <= fmax;
  };

  const auto PasswordIsValid2 = [](const auto& p) {
    int fmin, fmax;
    char c;
    std::string pwd;
    std::tie(fmin, fmax, c, pwd) = p;

    // No support for zero-index, lets make it zero-indexed
    fmin--;
    fmax--;

    return (pwd[fmin] == c) ^ (pwd[fmax] == c);
  };

  const auto LoadInput = [](auto& f) {
    int valid1 = 0;
    int valid2 = 0;
    std::string line;
    while (aoc::getline(f, line)) {
      std::string_view s(line);
      std::string_view part;
      int index = 0;
      int m = -1;
      int x = -1;
      char c = 0;
      std::string pwd;
      while (aoc::getline(s, part, "- :")) {
        switch (index) {
          case 0: m = aoc::stoi(part); break;
          case 1: x = aoc::stoi(part); break;
          case 2: assert(part.size() == 1); c = part[0]; break;
          case 3: pwd = part; break;
          default: assert(false); break;
        }
        index++;
      }

      if (index != 4) { continue; }

      Password p{m, x, c, pwd};
      const auto v1 = PasswordIsValid(p);
      const auto v2 = PasswordIsValid2(p);
      DEBUG_PRINT("Password: " << std::get<3>(p) << " Valid-1: " << v1 << " Valid-2: " << v2);
      valid1 += v1;
      valid2 += v2;
    }
    return std::make_pair(valid1, valid2);
  };
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  std::pair<int, int> input;
  if (inTest) {
    input = LoadInput(SampleStream);
  } else {
    auto f = aoc::open_argv_1(argc, argv);
    input = LoadInput(f);
  }

  int part1 = 0;
  int part2 = 0;

  std::tie(part1, part2) = input;

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

