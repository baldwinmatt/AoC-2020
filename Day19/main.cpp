#include "aoc/helpers.h"

#include <vector>
#include <map>

namespace {
  using Result = std::pair<int, int>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view SampleInput(R"(0: 4 1 5
1: 2 3 | 3 2
2: 4 4 | 5 5
3: 4 5 | 5 4
4: "a"
5: "b"

ababbb
bababa
abbbab
aaabbb
aaaabbb)");
  constexpr int SR_Part1 = 2;
  constexpr int SR_Part2 = 0;

  using RuleList = std::vector<int>;

  class Rule {
    int _id;
    char _char;

    RuleList _left_rules;
    RuleList _right_rules;

  public:
    Rule(std::string_view line)
        : _char(0) {

      // id:
      std::string_view part;
      aoc::getline(line, part, ':');
      _id = aoc::stoi(part);

      // if next token starts with '"', its a leaf
      aoc::getline(line, part, ' ');
      if (part[0] == '"') {
        assert(part.size() == 3);
        _char = part[1];
        return;
      }

      // else its a twig or branch
      constexpr std::string_view PIPE("|");
      auto &vec = _left_rules;
      do {
        if (part == PIPE) {
          // if we find a '|' its a branch
          vec = _right_rules;
          continue;
        }

        vec.push_back(aoc::stoi(part));
      } while (aoc::getline(line, part, " "));

      assert(isValid());
    }

    static std::pair<bool, std::string_view> matchesRules(std::string_view s, const std::map<int, Rule>& rules, const RuleList& list) {
      std::pair<bool, std::string_view> res = std::make_pair(false, s);
      for (const auto& id : list) {
        const auto rit = rules.find(id);
        assert (rit != rules.end());

        res = rit->second.matches(s, rules);
        DEBUG_PRINT("? " << s << " : " << rit->second._id << " = " << res.first);
        if (!res.first) {
          break;
        }
        s = res.second;
      }
      return res;
    }

    std::pair<bool, std::string_view> matches(std::string_view s, const std::map<int, Rule>& rules) const {
      if (s.empty()) { return std::make_pair(true, s); }

      if (isLeaf()) {
        return std::make_pair((s[0] == _char), s.substr(1));
      }

      std::pair<bool, std::string_view> r = Rule::matchesRules(s, rules, _left_rules);

      if (!r.first && isBranch()) {
        return Rule::matchesRules(s, rules, _right_rules);
      }

      return r;
    }

    int id() const {
      return _id;
    }

    static bool isRule(const std::string_view v) {
      return v.find(':') != std::string_view::npos;
    }

    bool isLeaf() const {
      return _char != 0;
    }

    bool isTwig() const {
      return !_left_rules.empty() && _right_rules.empty();
    }

    bool isBranch() const {
      return !_right_rules.empty() && _left_rules.empty(); 
    }

    bool isValid() const {
      return isLeaf() ^ isTwig() ^ isBranch();
    }
  };

  using Rules = std::map<int, Rule>;

  const auto LoadInput = [](auto f) {
    Result r{0, 0};
    std::string_view line;
    Rules rules;

    while (aoc::getline(f, line)) {
      if (!Rule::isRule(line)) {
        break;
      }
      Rule rule(line);

      rules.emplace(rule.id(), std::move(rule));
    }

    const auto& rootIt = rules.find(0);
    assert(rootIt != rules.end());
    const Rule& root = rootIt->second;

    do {
      const auto res = root.matches(line, rules);
      r.first += res.first;

      DEBUG_PRINT("Matches: " << res.first << " Input: " << line << " Left: " << res.second);
    } while (aoc::getline(f, line));

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

