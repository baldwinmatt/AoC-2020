#include "aoc/helpers.h"
#include <map>
#include <vector>

namespace {
  using Result = std::pair<int, int>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view SampleInput(R"(0,3,6)");
  constexpr int SR_Part1 = 436;
  constexpr int SR_Part2 = 175594;

  using Recitals = std::map<int, int>;
  using Spoken = std::vector<int>;
  using Input = std::tuple<Recitals, Spoken>;
  const auto LoadInput = [](auto f) {
    Input r{};
    int count = 0;
    aoc::parse_as_integers(f, ",", [&](int i) {
      std::get<0>(r).emplace(i, count);
      std::get<1>(r).push_back(i);
      count++;
    });

    return r;
  };

  const auto Recitation = [](Recitals& last_occurence, Spoken& numbers, int cur, int end) {
    for (int i = cur; i < end; i++) {
      int last = numbers.back();
      // try an emplace, as if we fail, we will get a reference to the existing item, so we get a find for free
      auto it = last_occurence.emplace(last, i - 1);
      if (it.second) {
        // first occurence
        numbers.push_back(0);
      } else {
        const auto next = i - it.first->second - 1;
        numbers.push_back(next);
        it.first->second = i - 1;
      }

      DEBUG_PRINT(i << ": " << last << " -> " << numbers.back());
    }

    return numbers.back();
  };
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

  int part1 = 0;
  int part2 = 0;

  Recitals& rec = std::get<0>(r);
  Spoken& numbers = std::get<1>(r);
  int last = numbers.back();

  rec.erase(last);

  part1 = Recitation(rec, numbers, numbers.size(), 2020);
  part2 = Recitation(rec, numbers, numbers.size(), 30000000);

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

