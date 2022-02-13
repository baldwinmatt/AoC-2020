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
    DEBUG_PRINT("Departure: " << input.first);

    while (aoc::getline(f, line, ",")) {
      constexpr std::string_view OOS("x");
      const int bus_id = line == OOS ? 0 : aoc::stoi(line);
      input.second.push_back(bus_id);
      DEBUG_PRINT("Bus ID: " << bus_id);
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
  using GCD = std::tuple<int64_t, int64_t, int64_t>;

  std::ostream& operator<<(std::ostream& os, const Constraint& c) {
    os << "{ " << c.first << ", " << c.second << " }";
    return os;
  }

  std::ostream& operator<<(std::ostream& os, const GCD& c) {
    os << "{ " << std::get<0>(c) << ", " << std::get<1>(c) << ", " << std::get<2>(c) << " }";
    return os;
  }

  const auto FindEarliestOffset = [](const Input& input) {

    size_t time = 0;
    size_t step = 1;

    // convert input into constraints
    Constraints constraints;
    for (size_t i = 0; i < input.second.size(); i++) {
      if (!input.second[i]) { continue; }
      constraints.push_back({i, input.second[i]});
      DEBUG_PRINT("Constraint: " << constraints.back());
    }

    for (const auto &b : constraints) {
      while ((time + b.first) % b.second != 0) { time += step; }

      step *= b.second;
    }

    return time;
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

