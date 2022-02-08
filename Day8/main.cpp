#include "aoc/helpers.h"

#include <vector>
#include <map>

namespace {
  using MappedFileSource = aoc::MappedFileSource<char>;

  constexpr std::string_view SampleInput(R"(nop +0
acc +1
jmp +4
acc +3
jmp -3
acc -99
acc +1
jmp -4
acc +6)");
  constexpr int SR_Part1 = 5;
  constexpr int SR_Part2 = 8;

  enum class Opcode {
    ACC,
    JMP,
    NOP,
  };

  const std::map<std::string, Opcode, std::less<>> OpCodeMap{
    { "acc", Opcode::ACC, },
    { "jmp", Opcode::JMP, },
    { "nop", Opcode::NOP, },
  };
  const auto GetOpcode = [](const std::string_view o) {
    const auto r = OpCodeMap.find(o);
    assert(r != OpCodeMap.end());
    if (r == OpCodeMap.end()) {
      throw std::runtime_error("Invalid opcode");
    }
    return r->second;
  };

  using Instruction = std::tuple<Opcode, int, bool>;
  using Program = std::vector<Instruction>;

  const auto LoadInput = [](auto f) {
    Program prog;
    std::string_view line;
    while (aoc::getline(f, line)) {
      const auto n = line.find(' ');
      assert(n != std::string_view::npos);
      if (n == std::string_view::npos) { continue; }

      std::string_view s = line.substr(0, n);
      const auto op = GetOpcode(s);

      line = line.substr(n + 1);
      const char sign = line[0];
      assert(sign == '+' || sign == '-');
      if (sign != '+' && sign != '-') { continue; }

      line = line.substr(1);
      const auto arg = aoc::stoi(line) * (1 - (2 * (sign == '-')));

      DEBUG_PRINT(s << " " << arg);

      prog.emplace_back(op, arg, false);
    }
    return prog;
  };

  enum class TermCode {
    InfiniteLoop,
    OK,
    Error,
  };

  TermCode RunProgram(Program program, int& acc) {
    int pc = 0;
    acc = 0;
    const int pc_max = program.size();

    while (pc < pc_max && pc >= 0) {
      auto& inst = program[pc];

      const auto arg = std::get<1>(inst);
      auto& ex = std::get<2>(inst);
      if (ex) {
        return TermCode::InfiniteLoop;
      }
      ex = true;

      switch (std::get<0>(inst)) {
        case Opcode::ACC:
          acc += arg;
          pc++;
          break;
        case Opcode::NOP:
          pc++;
          break;
        case Opcode::JMP:
          pc += arg;
          break;
      }
    }
    return TermCode::OK;
  }
}

int main(int argc, char** argv) {
  aoc::AutoTimer t;
  const bool inTest = argc < 2;

  Program r;
  if (inTest) {
    r = LoadInput(SampleInput);
  } else {
    std::unique_ptr<MappedFileSource>m(new MappedFileSource(argc, argv));
    std::string_view f(m->data(), m->size());
    r = LoadInput(f);
  }

  int part1;
  {
    const auto res = RunProgram(r, part1);
    assert(res == TermCode::InfiniteLoop);
    (void)res;
  }

  int part2 = 0;
  for (size_t i = 0; i < r.size(); i++) {
    if (std::get<0>(r[i]) == Opcode::JMP) {
      Program mod = r;
      std::get<0>(mod[i]) = Opcode::NOP;
      const auto res = RunProgram(mod, part2);
      if (res == TermCode::OK) {
        break;
      }
    }
  }

  aoc::print_results(part1, part2);

  if (inTest) {
    aoc::assert_result(part1, SR_Part1);
    aoc::assert_result(part2, SR_Part2);
  }

  return 0;
}

