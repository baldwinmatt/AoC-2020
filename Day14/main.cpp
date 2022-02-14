#include "aoc/helpers.h"

#include <array>
#include <iterator>
#include <numeric>
#include <vector>
#include <unordered_map>

namespace {
  using Result = std::pair<int64_t, int64_t>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view SampleInput(R"(mask = 000000000000000000000000000000X1001X
mem[42] = 100
mask = 00000000000000000000000000000000X0XX
mem[26] = 1)");
  constexpr int SR_Part1 = 51;
  constexpr int SR_Part2 = 208;

  using MaskPair = std::pair<bool, bool>;
  using CurrentMask = std::vector<MaskPair>;
  using Memory = std::unordered_map<int64_t, int64_t>;
  using BinaryNum = std::array<bool, 36>;
  using MemMaskSet = std::vector<int64_t>;

  std::ostream& operator <<(std::ostream& os, const CurrentMask& x) {
    for (auto it = x.begin(); it != x.end(); it++) {
      if (!it->first) { os << "X"; }
      else { os << it->second; }
    }
    return os;
  }

  std::ostream& operator <<(std::ostream& os, const BinaryNum& x) {
    for (const auto& it : x) {
      os << it;
    }
    return os;
  }

  const auto ToBinaryNum = [](const int64_t in) {
    BinaryNum b;
    b.fill(false);
    int64_t val = in;
    for (size_t i = 0; i < b.max_size(); i++) {
      const bool r = val & 1;
      val >>= 1;
      b[i] = r;
    }
    DEBUG_PRINT(in << " -> " << b);
    return b;
  };

  const auto ApplyMask = [](const CurrentMask& mask, int64_t val) {
    BinaryNum bn = ToBinaryNum(val);

    assert(mask.size() == 36);
    for (size_t i = 0; i < bn.size(); i++) {
      if (mask[i].first) { bn[i] = mask[i].second; }
    }

    int64_t out = 0;
    int64_t base = 1;
    for (size_t i = 0; i < bn.size(); i++) {
      if (bn[i]) { out += base; }
      base *= 2;
    }

    DEBUG_PRINT(val << " & " << mask << ": " << bn << " -> " << out);

    return out;
  };

  const auto GenerateMaskSet = [](int64_t& address, const CurrentMask& mask) {
    MemMaskSet r;

    int64_t addr = 0;
    int64_t base = 1;
    for (const auto& i : mask) {
      if (i.first && i.second) { // 1
        addr += base;
      } else if (!i.first && !i.second) { // X
        r.push_back(base);
      } else {
        addr += (address & base) ? base : 0;
      }

      base *= 2;
    }

    DEBUG_PRINT("In: " << address << " Out: " << addr << " Mask: " << mask << " Perms: " << r.size());

    address = addr;

    return r;
  };

  void ApplyMemMask(size_t depth, int64_t address, const MemMaskSet& maskSet, Memory& mem, int64_t val) {
    const int64_t base = maskSet[depth];
    const int64_t next = address + base;
    const size_t max_depth = maskSet.size() - 1;

    if (depth == max_depth) {
      {
        DEBUG_PRINT("mem[" << address << "] = " << val);
        auto rit = mem.emplace(address, 0);
        rit.first->second = val;
      }
      {
        DEBUG_PRINT("mem[" << next << "] = " << val);
        auto rit = mem.emplace(next, 0);
        rit.first->second = val;
      }

      return;
    }
    depth++;

    ApplyMemMask(depth, address, maskSet, mem, val);
    ApplyMemMask(depth, next, maskSet, mem, val);
  }

  const auto LoadInput = [](auto f) {
    Result r{0, 0};
    std::string_view line;
    constexpr std::string_view MASK("mask = ");
    constexpr std::string_view MEM("mem[");

    CurrentMask mask;
    Memory mem;
    Memory mem2;

    while (aoc::getline(f, line)) {
      if (aoc::starts_with(line, MASK)) {
        mask.clear();
        const auto mbits = line.substr(MASK.size());
        assert(mbits.size() == 36);

        mask.reserve(mbits.size());
        for (const auto &c : mbits) {
          switch (c) {
            case 'X':
              mask.emplace_back(false, false);
              break;
            case '1':
              mask.emplace_back(true, true);
              break;
            case '0':
              mask.emplace_back(true, false);
              break;
            default:
              throw std::runtime_error("Invalid input");
          }
        }
        std::reverse(mask.begin(), mask.end());
      } else if (aoc::starts_with(line, MEM)) {
        auto n = MEM.size();
        while (n < line.size() && line[n] >= '0' && line[n] <= '9') { n++; }
        assert(line[n] == ']');

        auto address = aoc::stoi(line.substr(MEM.size(), n - MEM.size()));
        while (n < line.size() && !(line[n] >= '0' && line[n] <= '9')) { n++; }
        assert((line[n] >= '0' && line[n] <= '9'));
        const auto val = aoc::stoi(line.substr(n));

        // Part 1
        {
          auto rit = mem.emplace(address, 0);
          rit.first->second = ApplyMask(mask, val);
        }

        // Part 2
        const auto maskSet = GenerateMaskSet(address, mask);
        ApplyMemMask(0, address, maskSet, mem2, val);
      }
    }

    r.first = std::accumulate(mem.begin(), mem.end(), r.first, [](const auto& s, const auto& v) { return s + v.second; });
    r.second = std::accumulate(mem2.begin (), mem2.end(), r.second, [](const auto& s, const auto& v) { return s + v.second; });

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

  int64_t part1 = 0;
  int64_t part2 = 0;

  std::tie(part1, part2) = r;

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

