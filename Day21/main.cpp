#include "aoc/helpers.h"

#include <list>
#include <set>
#include <map>
#include <algorithm>
#include <iterator>

namespace {
  using Result = std::pair<int, int>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view SampleInput(R"(mxmxvkd kfcds sqjhc nhms (contains dairy, fish)
trh fvjkl sbzzf mxmxvkd (contains dairy)
sqjhc fvjkl (contains soy)
sqjhc mxmxvkd sbzzf (contains fish))");
  constexpr size_t SR_Part1 = 5;
  constexpr std::string_view SR_Part2{"mxmxvkd,sqjhc,fvjkl"};

  using IngrediantMap = std::map<std::string, size_t, std::less<>>;
  using IngrediantList = std::set<std::string, std::less<>>;
  using Alergens = std::map<std::string, IngrediantList, std::less<>>;

  constexpr std::string_view CONTAINS("contains");
  const auto LoadInput = [](auto f) {
    IngrediantMap im{};
    Alergens al{};
    std::string_view line;
    while (aoc::getline(f, line)) {
      std::string_view p;
      IngrediantList il{};
      while (aoc::getline(line, p, " (")) {
        if (p == CONTAINS) {
          while (aoc::getline(line, p, ", )")) {
            auto ar = al.emplace(p, il);
            if (!ar.second) {
              IngrediantList ol{};
              std::set_intersection(ar.first->second.begin(), ar.first->second.end(), il.begin(), il.end(), std::inserter(ol, ol.begin()));
              std::swap(ar.first->second, ol);
            }
          }
          break;
        }
        il.emplace(p);
        auto r = im.emplace(p, 0);
        r.first->second++;
      }
    }
    return std::make_pair(im, al);
  };
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  std::pair<IngrediantMap, Alergens>r;
  if (inTest) {
    r = LoadInput(SampleInput);
  } else {
    std::unique_ptr<MappedFileSource>m(new MappedFileSource(argc, argv));
    std::string_view f(m->data(), m->size());
    r = LoadInput(f);
  }

  size_t part1 = 0;
  std::string part2 = "";

  IngrediantList alergens{};
  for (const auto& i : r.second) {
    for (const auto& a : i.second) {
      alergens.emplace(a);
    }
  }

  for (const auto& i : r.first) {
    const auto a = alergens.find(i.first);
    if (a == alergens.end()) {
      DEBUG_PRINT(i.first << " occurred " << i.second);
      part1 += i.second;
    }
  }

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

