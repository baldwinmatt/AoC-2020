#include "aoc/helpers.h"

#include <vector>

namespace {
  using Input = std::pair<int, int>;

  const std::string SampleInput(R"(ecl:gry pid:860033327 eyr:2020 hcl:#fffffd
byr:1937 iyr:2017 cid:147 hgt:183cm

iyr:2013 ecl:amb cid:350 eyr:2023 pid:028048884
hcl:#cfa07d byr:1929

hcl:#ae17e1 iyr:2013
eyr:2024
ecl:brn pid:760753108 byr:1931
hgt:179cm

hcl:#cfa07d eyr:2025 pid:166559648
iyr:2011 ecl:brn hgt:59in)");
  std::stringstream SampleStream(SampleInput);
  constexpr int SR_Part1 = 2;
  constexpr int SR_Part2 = 0;

  const auto LoadInput = [](auto& f) {
    Input input{ 0, 0 };
    std::string line;
    while (aoc::getline(f, line)) {
      //
    }

    return input;
  };
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  Input input;
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

