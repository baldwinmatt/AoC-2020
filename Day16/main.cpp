#include "aoc/helpers.h"

#include <vector>
#include <set>

namespace {
  using Result = std::pair<int64_t, int64_t>;
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view SampleInput(R"(class: 1-3 or 5-7
row: 6-11 or 33-44
seat: 13-40 or 45-50

your ticket:
7,1,14

nearby tickets:
7,3,47
40,4,50
55,2,20
38,6,12)");
  constexpr int SR_Part1 = 71;
  constexpr int SR_Part2 = 1;

  class Rule {
    std::string_view name;
    std::pair<int, int> range1;
    std::pair<int, int> range2;

    std::vector<size_t> indecies;

  public:
    Rule(std::string_view line) {
      aoc::getline(line, name, ":");

      std::string_view tmp;
      aoc::getline(line, tmp, " -");
      range1.first = aoc::stoi(tmp);
      aoc::getline(line, tmp, " ");
      range1.second = aoc::stoi(tmp);
      aoc::getline(line, tmp, " ");
      aoc::getline(line, tmp, " -");
      range2.first = aoc::stoi(tmp);
      aoc::getline(line, tmp, " ");
      range2.second = aoc::stoi(tmp);
    }

    bool check(int n) const {
      return (range1.first <= n && range1.second >= n) ||
        (range2.first <= n && range2.second >= n);
    }

    void populate_indicies(size_t num) {
      for (size_t i = 0; i < num; i++) {
        indecies.push_back(i);
      }
    }

    bool remove_index(size_t i) {
      auto r = std::find(indecies.begin(), indecies.end(), i);
      if (r != indecies.end()) {
        DEBUG_PRINT(name << " removing " << i);
        indecies.erase(r);
        return true;
      }
      return false;
    }

    bool solved() const {
      DEBUG_PRINT(name << " solved: " << (indecies.size() == 1));
      return indecies.size() == 1;
    }

    size_t prune(std::vector<Rule>& rules) {
      auto idx = indecies.front();
      size_t ret = 0;
      for (auto& r : rules) {
        if (r.name == this->name) { continue; }
        ret += r.remove_index(idx);
      }
      return ret;
    }

    size_t index() const {
      assert(indecies.size() == 1);

      return indecies.front();
    }

    std::string_view get_name() const {
      return name;
    }
  };

  class Ticket {
    std::vector<int> ids;

  public:
    Ticket(std::string_view line) {
      std::string_view tmp;
      while (aoc::getline(line, tmp, ",")) {
        ids.push_back(aoc::stoi(tmp));
      }
    }

    bool validate(const std::vector<Rule>& rules, int64_t& err) const {
      bool valid = false;
      for (const auto& n : ids) {
        valid = false;
        for (const auto& r : rules) {
          if (r.check(n)) {
            valid = true;
            break;
          }
        }
        if (!valid) {
          err += n;
          break;
        }
      }
      return valid;
    }

    void reduce_rule(Rule& r) {
      for (size_t i = 0; i < ids.size(); i++) {
        const int n = ids[i];
        if (!r.check(n)) {
          r.remove_index(i);
        }
      }
    }

    int get_id(const size_t i) const {
      return ids[i];
    }
  };

  using Rules = std::vector<Rule>;
  using Tickets = std::vector<Ticket>;

  const auto LoadInput = [](auto f) {
    Result r{0, 0};
    Rules rules;

    std::string_view line;
    while (aoc::getline(f, line)) {
      if (line == "your ticket:") {
        break;
      }
      DEBUG_PRINT("Rule: " << line);
      rules.emplace_back(line);
    }

    const size_t rule_count = rules.size();

    for (auto& r : rules) {
      r.populate_indicies(rule_count);
    }

    assert(line == "your ticket:");
    aoc::getline(f, line);
    Ticket my_ticket(line);
    DEBUG_PRINT("My ticket: " << line);

    aoc::getline(f, line);
    assert(line == "nearby tickets:");

    while (aoc::getline(f, line)) {
      Ticket t(line);
      if (t.validate(rules, r.first)) {
        DEBUG_PRINT("Valid ticket: " << line);
        for (auto& r : rules) {
          t.reduce_rule(r);
        }
      }
    }

    bool done = false;
    while (!done) {
      done = true;
      int pruned = 0;

      for (auto& r : rules) {
        if (!r.solved()) {
          done = false;
          continue;
        }

        pruned += r.prune(rules);
      }

      assert(pruned || done);
      if (!pruned && !done) {
        throw std::runtime_error("Unsolvable input");
      }
    }
    r.second = 1;
    constexpr std::string_view departure("departure");
    for (const auto& rule : rules) {
      DEBUG_PRINT(rule.get_name() << " : " << my_ticket.get_id(rule.index()));
      if (aoc::starts_with(rule.get_name(), departure)) {
        r.second *= my_ticket.get_id(rule.index());
      }
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

