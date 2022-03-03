#include "aoc/helpers.h"

#include <algorithm>
#include <vector>
#include <queue>
#include <map>
#include <set>

namespace {
  using Result = std::pair<int64_t, int64_t>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view Monster(R"(                  # 
#    ##    ##    ###
 #  #  #  #  #  #   )");

  constexpr std::string_view SampleInput(R"(Tile 2311:
..##.#..#.
##..#.....
#...##..#.
####.#...#
##.##.###.
##...#.###
.#.#.#..##
..#....#..
###...#.#.
..###..###

Tile 1951:
#.##...##.
#.####...#
.....#..##
#...######
.##.#....#
.###.#####
###.##.##.
.###....#.
..#.#..#.#
#...##.#..

Tile 1171:
####...##.
#..##.#..#
##.#..#.#.
.###.####.
..###.####
.##....##.
.#...####.
#.##.####.
####..#...
.....##...

Tile 1427:
###.##.#..
.#..#.##..
.#.##.#..#
#.#.#.##.#
....#...##
...##..##.
...#.#####
.#.####.#.
..#..###.#
..##.#..#.

Tile 1489:
##.#.#....
..##...#..
.##..##...
..#...#...
#####...#.
#..#.#.#.#
...#.#.#..
##.#...##.
..##.##.##
###.##.#..

Tile 2473:
#....####.
#..#.##...
#.##..#...
######.#.#
.#...#.#.#
.#########
.###.#..#.
########.#
##...##.#.
..###.#.#.

Tile 2971:
..#.#....#
#...###...
#.#.###...
##.##..#..
.#####..##
.#..####.#
#..#.#..#.
..####.###
..#.#.###.
...#.#.#.#

Tile 2729:
...#.#.#.#
####.#....
..#.#.....
....#..#.#
.##..##.#.
.#.####...
####.#.#..
##.####...
##..#.##..
#.##...##.

Tile 3079:
#.#.#####.
.#..######
..#.......
######....
####.#..#.
.#...#.##.
#.#####.##
..#.###...
..#.......
..#.###...)");

  constexpr int64_t SR_Part1 = 20899048083289;
  constexpr int64_t SR_Part2 = 0;

  using BitImage = std::vector<bool>;

  using Point = aoc::Point;
  using SparseImage = std::set<Point>;

  class Image {
    SparseImage _image;
    size_t _width;
    size_t _height;

  public:
    Image()
      : _width(0)
      , _height(0)
    { }

    Image(std::string_view source)
      : Image()
    {
      std::string_view line;
      while (aoc::getline(source, line)) {
        if (!_width) { _width = line.size(); }
        assert(line.size() == _width);
        for (size_t x = 0; x < line.size(); x++) {
          switch(line[x]) {
            case '#':
              _image.emplace(x, _height);
              break;
            case ' ':
            case '.':
              break;
            default:
              assert(false);
          }
        }
        _height++;
      }
    }

    const SparseImage& image() const {
      return _image;
    }

    const Point dims() const {
      return { _width, _height };
    }

    size_t size() const {
      return _image.size();
    }
  };
  

  class Tile {
    int64_t _id;
    size_t _width;
    size_t _height;
    size_t _set;
    BitImage _image;
    BitImage _original;
    Tile* _to_right;
    Tile* _to_left;
    Tile* _to_bottom;
    Tile* _to_top;

  public:
    Tile()
      : _id(0)
      , _width(0)
      , _height(0)
      , _set(0)
      , _to_right(nullptr)
      , _to_left(nullptr)
      , _to_bottom(nullptr)
      , _to_top(nullptr)
    {
    }

    Tile(std::string_view s)
      : Tile() {
        std::string_view part;
        aoc::getline(s, part, " :");
        assert(part == std::string_view("Tile"));
        aoc::getline(s, part, " :");
        _id = aoc::stoi(part);

        assert(!aoc::getline(s, part, " :"));
    }

    Tile(BitImage&& image, size_t width, size_t height)
      : _id()
      , _width(width) 
      , _height(height)
      , _image(image)
      , _to_right(nullptr)
      , _to_left(nullptr)
      , _to_bottom(nullptr)
      , _to_top(nullptr)
    {
    }

    void parse(const std::string_view s) {
      if (!_width) {
        _width = s.size();
      }
      assert(_width == s.size());
      if (_width != s.size()) { throw std::runtime_error("Bad input"); }

      size_t offset = _original.size();
      _original.resize(offset + _width, false);

      for (const auto& c : s) {
        assert(c == '#' || c == '.');
        bool set = (c == '#');
        _original[offset++] = set;
        _set += set;
      }
      _height++;
    }

    void linkDown(Tile* o) {
      _to_bottom = o;
      o->_to_top = this;
    }

    void linkRight(Tile* o) {
      _to_right = o;
      o->_to_left = this;
    }

    Tile* toRight() const {
      return _to_right;
    }

    Tile* toBottom() const {
      return _to_bottom;
    }

    void linkUp(Tile* o) {
      _to_top = o;
      o->_to_bottom = this;
    }

    void linkLeft(Tile* o) {
      _to_left = o;
      o->_to_right = this;
    }

    bool ok() const { return _height > 0 && _width > 0; }

    size_t width() const { return _width; }
    size_t height() const { return _height; }

    int64_t id() const {
      return _id;
    }

    bool get(size_t x, size_t y) const {
      assert(y < _height);
      assert(x < _width);

      return _image.at(y * _width + x);
    }
#ifndef NDEBUG
    std::string top() const {
      std::string v;
      for (size_t i = 0; i < _width; i++) {
        v.append(get(i, 0) ? "#" : ".");
      }
      return v;
    }

    std::string bottom() const {
      std::string v;
      for (size_t i = 0; i < _width; i++) {
        v.append(get(i, _height - 1) ? "#" : ".");
      }
      return v;
    }

    std::string right() const {
      std::string v;
      for (size_t i = 0; i < _height; i++) {
        v.append(get(_width - 1, i) ? "#" : ".");
      }
      return v;
    }

    std::string left() const {
      std::string v;
      for (size_t i = 0; i < _height; i++) {
        v.append(get(0, i) ? "#" : ".");
      }
      return v;
    }
#else
    int top() const {
      int v = 0;
      int base = 1;
      for (size_t i = 0; i < _width; i++) {
        v += (base * get(i, 0));
        base <<= 1;
      }
      return v;
    }

    int bottom() const {
      int v = 0;
      int base = 1;
      for (size_t i = 0; i < _width; i++) {
        v += base * get(i, _height - 1);
        base <<= 1;
      }
      return v;
    }

    int right() const {
      int v = 0;
      int base = 1;
      for (size_t i = 0; i < _height; i++) {
        v += base * get(_width - 1, i);
        base <<= 1;
      }
      return v;
    }

    int left() const {
      int v = 0;
      int base = 1;
      for (size_t i = 0; i < _height; i++) {
        v += base * get(0, i);
        base <<= 1;
      }
      return v;
    }
#endif

    void permute(int permutation) {
      _image = _original;
      switch (permutation) {
        case 0: flipHorizontal(); flipVertical(); break;
        case 1: flipHorizontal(); rotate(); break;
        case 2: flipVertical(); rotate(); break;
        case 3: rotate(); break;
        case 4: flipHorizontal(); break;
        case 5: flipVertical(); break;
        case 6: break;
        case 7: flipHorizontal(); flipVertical(); rotate(); break;
        default : throw std::runtime_error("Bad permutation"); break;
      }
    }

    void flipHorizontal() {
      for (size_t y = 0; y < _height; y++) {
        std::reverse(_image.begin() + (y * _width), _image.begin() + ((y + 1) * _width));
      }
    }

    void flipVertical() {
      for (size_t y = 0; y < _height / 2; y++) {
        for (size_t x = 0; x < _width; x++) {
          const size_t from = y * _width + x;
          const size_t to = (_height - y - 1) * _width + x;
          std::swap(_image[from], _image[to]);
        }
      }
    }

    void rotate() {
      for (size_t y = 0; y < _height - 1; y++) {
        for (size_t x = y + 1; x < _width; x++) {
          const size_t from = y * _width + x;
          const size_t to = x * _width + y;
          std::swap(_image[from], _image[to]);
        }
        std::reverse(_image.begin() + (y * _width), _image.begin() + ((y + 1) * _width));
      }
      std::reverse(_image.begin() + ((_height - 1) * _width), _image.end());

      std::swap(_height, _width);
    }

  public:

    void copyRowTo(size_t y, BitImage& dst, size_t offset) {
      size_t start = (y * _width) + 1;
      size_t end = start + _width - 2;
      std::copy(_image.begin() + start, _image.begin() + end, dst.begin() + offset);
    }

    bool isCorner() const {
      int links = !!_to_right;
      links += !!_to_top;
      links += !!_to_bottom;
      links += !!_to_left;

      return links == 2;
    }

    bool isKeyStone() const {
      return !_to_top && !_to_left &&
        !!_to_right && !!_to_bottom;
    }

    size_t set() const {
      return _set;
    }

    std::ostream& print(std::ostream& os) const {
      for (size_t y = 0; y < height(); y++) {
        for (size_t x = 0; x < width(); x++) {
          if(get(x, y)) {
            os << "#";
          } else {
            os << ".";
          }
        }
        os << std::endl;
      }
      os << std::endl;
      return os;
    }
  };

  std::ostream& operator<<(std::ostream& os, const Tile& t) {
    return t.print(os);
  }
  
  using TileList = std::vector<Tile>;
  using TileQueue = std::queue<Tile*>;

  const auto LoadInput = [](auto f) {
    std::string_view line;

    TileList tiles;
    while (aoc::getline(f, line)) {
      if (line[0] == 'T' && line.back() == ':') {
        tiles.emplace_back(line);
        continue;
      }

      assert(!tiles.empty());

      tiles.back().parse(line);
    }

    for (auto& t : tiles) {
      t.permute(6);
    }
    return tiles;
  };
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  TileList r;
  if (inTest) {
    r = LoadInput(SampleInput);
  } else {
    std::unique_ptr<MappedFileSource>m(new MappedFileSource(argc, argv));
    std::string_view f(m->data(), m->size());
    r = LoadInput(f);
  }

  using TileMap = std::map<int64_t, int>;
  TileQueue queue;
  TileMap solved;
  solved.insert({r.front().id(), 0});
  queue.push(&r[0]);

  while (!queue.empty()) {
    Tile* t = queue.front();
    DEBUG_PRINT("Solving for " << t->id());

    auto it = solved.find(t->id());
    assert (it != solved.end());

    t->permute(it->second);

    for (size_t n = 0; n < r.size(); n++) {
      Tile* other = &r[n];
      auto s = solved.emplace(other->id(), -1);
      if (s.first->second != -1) { continue; }

      for (int i = 0; i < 8; i++) {
        other->permute(i);
        if (t->left() == other->right() ||
          t->right() == other->left() ||
          t->top() == other->bottom()||
          t->bottom() == other->top())
        {
          s.first->second = i;
          DEBUG_PRINT("Match: " << t->id() << " with " << other->id() << " rot " << i);
          queue.push(other);
          break;
        }
      }
    }
    queue.pop();
  }

  int64_t part1 = 1;
  Tile* key = nullptr;

  for (auto& t : r) {
    for (auto& o : r) {
      if (t.left() == o.right()) {
        t.linkLeft(&o);
      } else if (t.right() == o.left()) {
        o.linkLeft(&t);
      } else if (t.top() == o.bottom()) {
        o.linkDown(&t);
      } else if (t.bottom() == o.top()) {
        t.linkDown(&o);
      }
    }
    if (t.isCorner()) {
      DEBUG_PRINT("Corner: " << t.id());
      part1 *= t.id();
      if (t.isKeyStone()) {
        key = &t;
      }
    }
  }

  // compose final image
  std::vector<std::string> image;

  size_t height = 0;
  size_t width = 0;
  Tile* l = key;
  while (l) {
    Tile *c = l;
    image.resize(height + l->height() - 2, std::string(""));
    while (c) {
      for (size_t x = 1; x < c->width() - 1; x++) {
        for (size_t y = 1; y < c->height() - 1; y++) {
          const char p = c->get(x, y) ? '#' : '.';
          image[height + y - 1].append(&p, 1);
        }
      }
      width += c->width() - 2;
      c = c->toRight();
    }
    height += l->height() - 2;
    l = l->toBottom();
  }

  Tile composedImage("Tile 0:");
  for (const auto& line : image) {
    composedImage.parse(line);
  }

  // Parse monster
  const Image monster{Monster};
  const Point dims = monster.dims();

  size_t monsters = 0;
  for (int perm = 0; !monsters && perm < 8; perm++) {
    composedImage.permute(perm);
    DEBUG_PRINT(composedImage);

    for (size_t x = 0; x < composedImage.width() - dims.first; x++) {
      for (size_t y = 0; y < composedImage.height() - dims.second; y++) {
        bool m = true;
        for (const auto& p : monster.image()) {
          m = composedImage.get(x + p.first, y + p.second);
          if (!m) { break; }
        }
        monsters += m;
      }
    }
  }
  int64_t part2 = composedImage.set() - (monsters * monster.size());

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

