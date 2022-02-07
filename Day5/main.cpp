#include "aoc/helpers.h"

#include <vector>

namespace {
  using Result = std::pair<int, int>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  using SeatMap = std::vector<bool>;

  constexpr std::string_view SampleInput(R"(FBFBBFFRLR
BFFFBBFRRR
FFFBBBFRRR
BBFFBBFRLL)");
  constexpr int SR_Part1 = 820;
  constexpr int SR_Part2 = 120;

  const auto GetSeatId = [](const auto seat) {

    int row = 128;
    int col = 8;

    std::pair<int, int> range{0, 127};

    size_t i = 0;
    while (i < 7) {
      const auto c = seat[i];
      const auto span = range.second - range.first + 1;
      switch (c) {
        case 'F':
          range.second -= (span / 2);
          row = range.second;
          break;
        case 'B':
          range.first += (span / 2);
          row = range.first;
          break;
        default:
          throw std::runtime_error("Invalid input");
      }
      i++;
    }

    assert(range.first == range.second);

    range = {0, 7};
    while (i < seat.size()) {
      const auto c = seat[i];
      const auto span = range.second - range.first + 1;
      switch (c) {
        case 'L':
          range.second -= (span / 2);
          col = range.second;
          break;
        case 'R':
          range.first += (span / 2);
          col = range.first;
          break;
        default:
          throw std::runtime_error("Invalid input");
      }
      i++;
    }

    assert(range.first == range.second);

    return row * 8 + col;
  };

  const auto LoadInput = [](auto f) {
    Result r{0, 0};
    std::string_view line;

    SeatMap map;
    
    while (aoc::getline(f, line)) {
      if (line.size() != 10) {
        continue;
      }

      const auto id = GetSeatId(line);

      r.first = std::max(id, r.first);
      map.resize(r.first + 1);
      map[id] = true;
    }

    int i = 0;
    while (!map[i]) { i++; }
    while (map[i]) { i++; }
    r.second = i;

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

