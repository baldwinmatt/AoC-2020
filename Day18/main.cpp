#include "aoc/helpers.h"

namespace {
  using Result = std::pair<int64_t, int64_t>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view SampleInput(R"(((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2)");
  constexpr int SR_Part1 = 13632;
  constexpr int SR_Part2 = 23340;

  enum class Precedence {
    Standard,
    Reversed,
  };

  const std::string Reduce(std::string_view eq, Precedence prec);

  const auto SolveEquation = [](const auto eq, Precedence prec) {
    
    const auto reduced = Reduce(eq, prec);
    std::string_view reduced_view{ reduced };
    int64_t l = 0;
    char op = '+';
    std::vector<int64_t>parts;
    
    std::string_view part;
    while (aoc::getline(reduced_view, part, ' ')) {
      constexpr std::string_view PLUS("+");
      constexpr std::string_view MULL("*");

      if (part == PLUS || part == MULL) {
        op = part[0];
      } else {
        const auto r = aoc::stoi(part);
        switch (op) {
          case '+':
            l += r;
            break;
          case '*':
            switch (prec) {
              case Precedence::Standard:
                l *= r;
                break;
              case Precedence::Reversed:
                parts.push_back(l);
                l = r;
                break;
            }
            break;
          default:
            throw std::runtime_error("Bad operator");
        }
      }
    }

    assert(parts.empty() || prec == Precedence::Reversed);

    for (const auto &r : parts) {
      l *= r;
    }

    return l;
  };

  const std::string Reduce(std::string_view eq, Precedence prec) {
    std::string eqs(eq);
    do {
      size_t l = 0;
      size_t r = 0;
      size_t depth = 0;
      size_t i = 0;
      bool has_parens = false;

      for (const auto &c : eqs) {
        switch (c) {
        case '(':
          if (!depth) { l = i; }
          has_parens = true;
          depth++;
          break;
        case ')':
          depth--;
          if (!depth) { r = i; }
        default:
          break;
        }
        i++;
      }

      if (!has_parens) {
        break;
      }
      const std::string_view eqv(eqs);
      const auto left = eqv.substr(0, l);
      const auto right = eqv.substr(r + 1);
      const auto inside = eqv.substr(l + 1, r - l - 1);

      const auto inner = std::to_string(SolveEquation(inside, prec));
      std::string next;
      next.reserve(left.size() + inner.size() + right.size());
      next.append(left).append(inner).append(right);

      DEBUG_PRINT(eqs << " -> " << next);

      std::swap(eqs, next);

    } while (true);

    return eqs;
  }

  const auto LoadInput = [](auto f) {
    Result r{0, 0};
    std::string_view line;
    while (aoc::getline(f, line)) {
      r.first += SolveEquation(line, Precedence::Standard);
      r.second += SolveEquation(line, Precedence::Reversed);
    }
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

