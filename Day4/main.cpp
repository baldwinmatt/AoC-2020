#include "aoc/helpers.h"

#include <set>
#include <vector>

namespace {
  using Input = std::pair<int, int>;
  using MappedFileSource = aoc::MappedFileSource<char>;

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
  constexpr int SR_Part1 = 2;
  constexpr int SR_Part2 = 2;

  constexpr std::string_view byr("byr");
  constexpr std::string_view iyr("iyr");
  constexpr std::string_view eyr("eyr");
  constexpr std::string_view hgt("hgt");
  constexpr std::string_view hcl("hcl");
  constexpr std::string_view ecl("ecl");
  constexpr std::string_view pid("pid");
  constexpr std::string_view cid("cid");

  struct Passport {
    std::string_view byr;
    std::string_view iyr;
    std::string_view eyr;
    std::string_view hgt;
    std::string_view hcl;
    std::string_view ecl;
    std::string_view pid;
    std::string_view cid;
  };

  std::ostream& operator<<(std::ostream& os, const Passport& p) {
    os << "cid: " << p.cid << " pid: " << p.pid << " ecl: " << p.ecl <<
      " hcl: " << p.hcl << " hgt: " << p.hgt << " eyr: " << p.eyr <<
      " iyr: " << p.iyr << " byr: " << p.byr;

      return os;
  }

  bool IsValid(const Passport& p) {
    return !p.byr.empty() &&
      !p.iyr.empty() &&
      !p.eyr.empty() &&
      !p.hgt.empty() &&
      !p.hcl.empty() &&
      !p.ecl.empty() &&
      !p.pid.empty();
  }

  const std::set<std::string, std::less<>> ValidEyeColors{
    { "amb" }, {"blu"}, {"brn"}, {"gry"}, {"grn"}, {"hzl"}, { "oth" }
  };

  bool IsDataValid(const Passport& p) {
    if (!IsValid(p)) { return false; }

    int v = aoc::stoi(p.byr);
    if (v < 1920 || v > 2002) { DEBUG_PRINT("invalid byr"); return false; }

    v = aoc::stoi(p.iyr);
    if (v < 2010 || v > 2020) { DEBUG_PRINT("invalid iyr"); return false; }

    v = aoc::stoi(p.eyr);
    if (v < 2020 || v > 2030) { DEBUG_PRINT("invalid eyr6"); return false; }

    if (aoc::ends_with(p.hgt, "in")) {
      v = aoc::stoi(p.hgt.substr(0, p.hgt.size() - 2));
      if (v < 59 || v > 76) { DEBUG_PRINT("invalid hgt: " << v); return false; }
    } else if (aoc::ends_with(p.hgt, "cm")) {
      v = aoc::stoi(p.hgt.substr(0, p.hgt.size() - 2));
      if (v < 150 || v > 193) { DEBUG_PRINT("invalid hgt: " << v); return false; }
    } else { DEBUG_PRINT("invalid hgt"); return false; }

    if (p.hcl.size() != 7 || p.hcl[0] != '#') { DEBUG_PRINT("invalid hcl"); return false; }
    for (const auto& c : p.hcl.substr(1)) {
      if ((c >= 'a' && c <= 'f') || (c >= '0' && c <= '9')) {
        continue;
      }
      DEBUG_PRINT("invalid hcl"); 
      return false;
    }

    if (ValidEyeColors.find(p.ecl) == ValidEyeColors.end()) {
      DEBUG_PRINT("invalid ecl"); return false;
    }

    if (p.pid.size() != 9) { DEBUG_PRINT("invalid pid"); return false; }
    for (const auto& c : p.pid.substr(1)) {
      if (c >= '0' && c <= '9') {
        continue;
      }
      DEBUG_PRINT("invalid pid"); 
      return false;
    }

    return true;
  }

  const auto LoadInput = [](std::string_view f) {
    Input input{ 0, 0 };

    Passport p;
    std::string_view line;
    while (aoc::getline(f, line, "\r\n ", true)) {
      if (line.empty()) {
        const auto v = IsValid(p);
        const auto dv = IsDataValid(p);

        DEBUG_PRINT(p << " Valid: " << v << " Data Valid: " << dv);
        input.first += v;
        input.second += dv;
        p = Passport();
        continue;
      }
      const auto pos = line.find(':');
      assert(pos != std::string_view::npos);
      if (pos == std::string_view::npos) { continue; }

      std::string_view field = line.substr(0, pos);
      std::string_view value = line.substr(pos + 1);

      if (field == byr) { p.byr = value; }
      else if (field == iyr) { p.iyr = value; }
      else if (field == eyr) { p.eyr = value; }
      else if (field == hgt) { p.hgt = value; }
      else if (field == hcl) { p.hcl = value; }
      else if (field == ecl) { p.ecl = value; }
      else if (field == pid) { p.pid = value; }
      else if (field == cid) { p.cid = value; }
      else { assert(false); }
    }

    {
        const auto v = IsValid(p);
        const auto dv = IsDataValid(p);

        DEBUG_PRINT(p << " Valid: " << v << " Data Valid: " << dv);
        input.first += v;
        input.second += dv;
      }
    
    return input;
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

