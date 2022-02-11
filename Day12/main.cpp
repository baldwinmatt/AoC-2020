#include "aoc/helpers.h"

#include <vector>

namespace {
  using Result = std::pair<int, int>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view SampleInput(R"(F10
N3
F7
R90
F11)");
  constexpr int SR_Part1 = 25;
  constexpr int SR_Part2 = 286;

  using Point = aoc::Point;

  enum class Direction {
    North,
    East,
    South,
    West,
  };

  const std::vector<Direction> DirectionSet = {
    Direction::North,
    Direction::South,
    Direction::West,
    Direction::East,
  };

  std::ostream& operator<<(std::ostream& os, const Direction& d) {
    switch (d) {
      case Direction::North:
        os << "N";
        break;
      case Direction::South:
        os << "S";
        break;
      case Direction::East:
        os << "E";
        break;
      case Direction::West:
        os << "W";
        break;
    }
    return os;
  }

  const std::vector<Point> Directions = {
      {  0,  1 },
      {  1,  0 },
      {  0, -1 },
      { -1,  0 },
  };

  const auto turn_left = [](Direction dir) {
    const int val = static_cast<int>(dir);
    return static_cast<Direction>((val + 3) & 3);
  };

  const auto turn_right = [](Direction dir) {
    const int val = static_cast<int>(dir);
    return static_cast<Direction>((val + 1) & 3);
  };

  const auto rotate_left = [](Point p) {
    return Point{ -p.second, p.first };
  };

  const auto rotate_right = [](Point p) {
    return Point{ p.second, -p.first };
  };

  const auto LoadInput = [](auto f) {
    Result r{0, 0};
    std::string_view line;

    Point pos{0, 0};
    Direction dir = Direction::East;

    Point wp{10, 1};
    Point ship{0, 0};

    while (aoc::getline(f, line)) {
      assert(line.size() > 1);
      if (line.size() < 2) { throw std::runtime_error("Invalid input"); }

      const char action = line[0];
      const int value = aoc::stoi(line.substr(1));

      switch (action) {
        case 'N':
          pos.second += value;
          wp.second += value;
          break;
        case 'S':
          pos.second -= value;
          wp.second -= value;
          break;
        case 'E':
          pos.first += value;
          wp.first += value;
          break;
        case 'W':
          pos.first -= value;
          wp.first -= value;
          break;
        case 'F':
          pos.first += (Directions[static_cast<int>(dir)].first * value);
          pos.second += (Directions[static_cast<int>(dir)].second * value);

          ship.first += wp.first * value;
          ship.second += wp.second * value;
          break;
        case 'R':
          {
            int rotations = value / 90;
            while (rotations) {
              dir = turn_right(dir);
              wp = rotate_right(wp);
              rotations--;
            }
          }
          break;
        case 'L':
          {
            int rotations = value / 90;
            while (rotations) {
              dir = turn_left(dir);
              wp = rotate_left(wp);
              rotations--;
            }
          }
          break;
        default:
          throw std::runtime_error("Bad Input");
      }
      DEBUG_PRINT(action << ":" << value << " -> " << pos.first << ", " << pos.second << " " << dir);
      DEBUG_PRINT(action << ":" << value << " WP -> " << wp.first << ", " << wp.second << " Ship -> " << ship.first << ", " << ship.second);
    }

    r.first = std::abs(pos.first) + std::abs(pos.second);
    r.second = std::abs(ship.first) + std::abs(ship.second);

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

