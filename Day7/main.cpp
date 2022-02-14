#include "aoc/helpers.h"

#include <map>
#include <set>

namespace {
  constexpr std::string_view NO("no");
  constexpr std::string_view SHINY_GOLD("shiny gold");

  using Result = std::pair<int, int>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  using BagMap = std::map<std::string, int, std::less<>>;
  
  struct Bag {
    BagMap contains;
    BagMap contained_by;
  };

  using BagsMap = std::map<std::string, Bag, std::less<>>;
  using BagSet = std::set<std::string, std::less<>>;

  const auto GetBagColor = [](auto& bag) {
    std::string_view color = bag;
    size_t n = 0;
    std::string_view part;
    size_t length = 0;
    while (n < 2 && aoc::getline(bag, part, ' ')) {
      if (!length) { length++; }
      length += part.size();
      n++;
    }

    return color.substr(0, length);
  };

  const auto ParseBagLine = [](auto& bagline, BagsMap& bags) {
    const auto color = GetBagColor(bagline);
    
    size_t pos = 2;
    std::string_view part;
    while (pos < 4 && aoc::getline(bagline, part, " ")) {
      pos++;
    }
    Bag b;
    auto f = bags.emplace(color, b);
    auto& bag = f.first->second;

    while (aoc::getline(bagline, part, ",.")) {
      std::string_view p;
      std::string col;
      aoc::getline(part, p, " ");
      if (p == NO) {
        DEBUG_PRINT(color << " is leaf");
        continue;
      }
      int num = aoc::stoi(p);
      col = GetBagColor(part);

      DEBUG_PRINT(color << " contains " << num << " " << col);
      bag.contains.insert(std::make_pair(col, num));
      f = bags.emplace(col, b);
      f.first->second.contained_by.insert(std::make_pair(std::string(color), num));
      DEBUG_PRINT(col << " contained_by " << color);
    }

    return bags;
  };

  constexpr std::string_view SampleInput(R"(light red bags contain 1 bright white bag, 2 muted yellow bags.
dark orange bags contain 3 bright white bags, 4 muted yellow bags.
bright white bags contain 1 shiny gold bag.
muted yellow bags contain 2 shiny gold bags, 9 faded blue bags.
shiny gold bags contain 1 dark olive bag, 2 vibrant plum bags.
dark olive bags contain 3 faded blue bags, 4 dotted black bags.
vibrant plum bags contain 5 faded blue bags, 6 dotted black bags.
faded blue bags contain no other bags.
dotted black bags contain no other bags.)");
  constexpr int SR_Part1 = 4;
  constexpr int SR_Part2 = 32;

  const auto LoadInput = [](auto f) {
    BagsMap rules;
    std::string_view line;
    while (aoc::getline(f, line)) {
      ParseBagLine(line, rules);
    }
    return rules;
  };

  void GetContainedBy(const std::string_view color, const BagsMap& bags, BagSet& set) {
    const auto r = bags.find(color);
    assert(r != bags.end());
    if (r == bags.end()) { return; }

    const auto& bag = r->second;
    for (const auto& c : bag.contained_by) {
      set.insert(c.first);
      GetContainedBy(c.first, bags, set);
    }
  }

  void GetContentsOf(const std::string_view color, const BagsMap& bags, int& total, int n) {
    const auto r = bags.find(color);
    assert(r != bags.end());
    if (r == bags.end()) { return; }

    const auto& bag = r->second;
    for (const auto& c : bag.contains) {
      const int num = (n * c.second);
      total += num;
      GetContentsOf(c.first, bags, total, num);
    }
  }
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  BagsMap r;
  if (inTest) {
    r = LoadInput(SampleInput);
  } else {
    std::unique_ptr<MappedFileSource>m(new MappedFileSource(argc, argv));
    std::string_view f(m->data(), m->size());
    r = LoadInput(f);
  }

  BagSet containers;
  GetContainedBy(SHINY_GOLD, r, containers);
  int part1 = containers.size();

  int part2 = 0;
  GetContentsOf(SHINY_GOLD, r, part2, 1);

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

