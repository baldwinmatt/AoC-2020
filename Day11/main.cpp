#include "aoc/helpers.h"

#include <vector>
#include <map>

namespace {
  using Result = std::pair<int, int>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view SampleInput(R"(L.LL.LL.LL
LLLLLLL.LL
L.L.L..L..
LLLL.LL.LL
L.LL.LL.LL
L.LLLLL.LL
..L.L.....
LLLLLLLLLL
L.LLLLLL.L
L.LLLLL.LL)");
  constexpr int SR_Part1 = 37;
  constexpr int SR_Part2 = 26;

  enum class Position {
    Floor,
    Empty,
    Occupied,
  };

  const auto GetPosition = [](const char c) {
    switch (c) {
      case '.':
        return Position::Floor;
      case 'L':
        return Position::Empty;
      case '#':
        return Position::Occupied;
      default:
        throw std::runtime_error("Invalid input");
    }
  };

  using PositionVector = std::vector<Position>;
  
  const std::vector<std::pair<int, int>> AdjacencyOffsets = {
    { -1, -1 },
    { -1, 0 },
    { -1, 1 },
    { 0, -1 },
    // {0, 0 },
    { 0, 1 },
    { 1, -1 },
    { 1, 0 },
    { 1, 1 },
  };

  class FloorPlan {
    PositionVector floor;
    size_t width;
    size_t height;
    size_t occupied;

  public:

    FloorPlan()
      : width(0)
      , height(0)
      , occupied(0)
    { }

    void parse(const std::string_view s) {
      if (!width) {
        width = s.size() + 2;
      }
      assert(width == s.size() + 2);
      if (width != s.size() + 2) { throw std::runtime_error("Bad input"); }

      size_t offset = floor.size();
      if (offset == 0) {
        floor.resize(width, Position::Floor);
        offset = width;
        height++;
      }
      floor.resize(offset + width, Position::Floor);
      offset++;

      for (const auto& c : s) {
        const auto p = GetPosition(c);
        floor[offset++] = p;
        occupied += (p == Position::Occupied);
      }
      height++;
    }

    void parse_done() {
      size_t offset = floor.size();
      floor.resize(offset + width, Position::Floor);
      height++;
    }

    Position get(size_t x, size_t y) const {
      return floor[index_of(x, y)];
    }

    bool iterate() {
      PositionVector next = floor;

      bool stable = true;

      for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
          switch (get(x, y)) {
            case Position::Floor:
              break;
            case Position::Empty:
              {
                const auto o = get_occupied_adjacent(x, y);
                // If a seat is empty (L) and there are no occupied seats adjacent to it,
                // the seat becomes occupied.
                if (!o) {
                  next[index_of(x, y)] = Position::Occupied;
                  occupied++;
                  stable = false;
                }
              }
              break;
            case Position::Occupied:
              {
                const auto o = get_occupied_adjacent(x, y);
                // If a seat is occupied (#) and four or more seats adjacent to it are
                // also occupied, the seat becomes empty.
                if (o >= 4) {
                  next[index_of(x, y)] = Position::Empty;
                  occupied--;
                  stable = false;
                }
              }
              break;
          }
        }
      }

      if (!stable) {
        floor.swap(next);
      }

      return stable;
    }

    bool iterate2() {
      PositionVector next = floor;

      bool stable = true;

      for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
          switch (get(x, y)) {
            case Position::Floor:
              break;
            case Position::Empty:
              {
                const auto o = get_occupied_adjacent2(x, y);
                // If a seat is empty (L) and there are no occupied seats adjacent to it,
                // the seat becomes occupied.
                if (!o) {
                  next[index_of(x, y)] = Position::Occupied;
                  occupied++;
                  stable = false;
                }
              }
              break;
            case Position::Occupied:
              {
                const auto o = get_occupied_adjacent2(x, y);
                // If a seat is occupied (#) and five or more seats adjacent to it are
                // also occupied, the seat becomes empty.
                if (o >= 5) {
                  next[index_of(x, y)] = Position::Empty;
                  occupied--;
                  stable = false;
                }
              }
              break;
          }
        }
      }

      if (!stable) {
        floor.swap(next);
      }

      return stable;
    }

    size_t occupied_count() const {
      return occupied;
    }

  private:
    size_t index_of(size_t x, size_t y) const {
      assert(x < width);
      assert(y < height);

      return y * height + x;
    }

    int get_occupied_adjacent(size_t x, size_t y) const {
      assert(x > 1 && x < width - 1);
      assert(y > 1 && y < height - 1);

      int count = 0;
      for (const auto &o : AdjacencyOffsets) {
        count += get(x + o.first, y + o.second) == Position::Occupied;
      }

      return count;
    }

    int get_occupied_adjacent2(size_t x, size_t y) const {
      assert(x > 1 && x < width - 1);
      assert(y > 1 && y < height - 1);

      int count = 0;
      for (const auto &o : AdjacencyOffsets) {
        size_t newx = x;
        size_t newy = y;
        do {
          newx += o.first;
          newy += o.second;
        } while (newx > 0 && newy > 0 &&
          newx < width - 1 && newy < height - 1 &&
          get(newx, newy) == Position::Floor);

        count += get(newx, newy) == Position::Occupied;
      }

      return count;
    }
  };

  const auto LoadInput = [](auto f) {
    FloorPlan fp;
    std::string_view line;
    while (aoc::getline(f, line)) {
      fp.parse(line);
    }
    fp.parse_done();
    return fp;
  };
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  FloorPlan fp;
  if (inTest) {
    fp = LoadInput(SampleInput);
  } else {
    std::unique_ptr<MappedFileSource>m(new MappedFileSource(argc, argv));
    std::string_view f(m->data(), m->size());
    fp = LoadInput(f);
  }

  // We'll need to start again with part2
  FloorPlan fp2 = fp;
  
  while (!fp.iterate());
  int part1 = fp.occupied_count();

  while (!fp2.iterate2());
  int part2 = fp2.occupied_count();

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

