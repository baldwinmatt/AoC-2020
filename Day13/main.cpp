#include "aoc/helpers.h"

#include <vector>
#include <numeric>

namespace {
  using Result = std::pair<int, int>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view SampleInput(R"(939
7,13,x,x,59,x,31,19)");
  constexpr int SR_Part1 = 295;
  constexpr int SR_Part2 = 1068781;

  using Input = std::pair<int, std::vector<int>>;

  const auto LoadInput = [](auto f) {
    Input input;
    std::string_view line;
    aoc::getline(f, line);
    input.first = aoc::stoi(line);

    while (aoc::getline(f, line, ",")) {
      constexpr std::string_view OOS("x");
      const int bid = line == OOS ? 0 : aoc::stoi(line);
      input.second.push_back(bid);
    }

    return input;
  };

  const auto FindEarliest = [](const Input& input) {
    int mod = 0;
    int earliest = -1;

    for (const auto& bid : input.second) {
      if (!bid) {
        continue;
      }

      const auto n = input.first % bid;
      if (n > mod) {
        mod = n;
        earliest = bid;
      }
    }

    const auto leave = input.first - (input.first % earliest) + earliest;
    const auto wait = leave - input.first;
    return wait * earliest;
  };

  using Constraint = std::pair<int64_t, int64_t>;
  using Constraints = std::vector<Constraint>;

  const auto XGCD = [](int64_t a, int64_t b) {
    int64_t x0 = 0;
    int64_t x1 = 1;
    int64_t y0 = 1;
    int64_t y1 = 0;

    while (a) {
      const int64_t _a = a;
      const int64_t q = b / a;

      a = ((b % a) + a) % a;
      b = _a;

      const int64_t _y0 = y0;
      y0 = y1;
      y1 = _y0 - q * y1;

      const int64_t _x0 = x0;
      x0 = x1;
      x1 = _x0 - q * x1;
    }

    return std::tuple{b, x0, y0};
  };

  const auto CRT = [](const Constraint& an, const Constraint& bn) {
    const int64_t a1 = an.first;
    const int64_t a2 = an.second;
    const int64_t n1 = bn.first;
    const int64_t n2 = bn.second;

    const auto gcd = XGCD(n1, n2);
    const int64_t m1 = std::get<1>(gcd);
    const int64_t m2 = std::get<2>(gcd);

    const int64_t x = (a1 * m2 * n2) + (a2 * m1 * n1);
    const int64_t n = n1 * n2;

    return std::make_pair(x % n, n);
  };

  const auto FindEarliestOffset = [](const Input& input) {
    // convert input into constraints
    Constraints constraints;
    constraints.reserve(input.second.size());
    for (size_t i = 0; i < input.second.size(); i++) {
      if (!input.second[i]) { continue; }
      constraints.emplace_back(i, input.second[i]);
    }

    const auto r = std::accumulate(constraints.begin() + 1, constraints.end(), constraints.front(), CRT);

    return r.second - r.first % r.second;
  };
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  Input input;
  if (inTest) {
    input = LoadInput(SampleInput);
  } else {
    std::unique_ptr<MappedFileSource>m(new MappedFileSource(argc, argv));
    std::string_view f(m->data(), m->size());
    input = LoadInput(f);
  }

  int part1 = FindEarliest(input);
  int64_t part2 = FindEarliestOffset(input);

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

