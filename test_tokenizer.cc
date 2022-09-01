#include "expression_calculus/expression.h"
#include "expression_calculus/parsing.h"

#include <iostream>
#include <cstdlib>
#include <numeric>
#include <random>

constexpr std::array<char, 5> WS{' ', '\t', '\r', '\f', '\v'};
std::mt19937 generator;

struct Test {
public:
  Test(std::vector<std::string> tokens, bool withoutSpaces = true) {
    std::string tokensString = std::accumulate(tokens.begin() + 1, tokens.end(), tokens.front(),
        [] (std::string acc, const std::string& s) {
          return std::move(acc) + " " + s;
        });
    constexpr std::size_t MAX_ITERATIONS = 10'000;
    std::cout << "Testing `" << tokensString << "`...(" << MAX_ITERATIONS << " iterations)..." << std::flush;

    const auto test = [&tokens] (const std::string& exprStr) {
      auto parser = std::make_unique<Parser>(exprStr);
      if (parser->GetTokens() != tokens) {
        std::cout << "Mismatch!" << std::endl;
        std::abort();
      }
    };

    if (withoutSpaces) {
      // 1. test whether tokens concatenated without delimiter are parsed
      // correctly
      test(std::accumulate(tokens.begin(), tokens.end(), std::string{}, std::plus<std::string>{}));
    }

    // 2. same test but now we put spaces between tokens
    const auto appendWs = [] (std::string& str) {
      do {
        // we need at least one space between tokens
        str += WS[std::uniform_int_distribution<>(0, WS.size() - 1)(generator)];
      } while (generator() % 3 > 0);
    };
    for (std::size_t t = 0; t < MAX_ITERATIONS; t++) {
      // Join the tokens and add some whitespace characters in-between
      std::string exprStr;
      appendWs(exprStr);
      for (const auto& token : tokens) {
        exprStr.append(token);
        appendWs(exprStr);
      }
      test(exprStr);
    }
  }

  ~Test() {
    std::cout << "Done" << std::endl;
  }
};

int main() {
  Test{{"A"}};
  Test{{"A", "->", "B"}};
  Test{{"A", "->", "B", "->", "C"}};
  Test{{"A", "->", "B", "|", "C", "->", "D"}};
  Test{{"A", "|", "B", "->", "C", "&", "D"}};
  Test{{"(", "(", "A", ")", ")", "->", "(", "B", "|", "(", "C", "|", "D", ")", ")"}};
  Test{{"A", "->", "B", "->", "A", "&", "B"}};
  Test{{"!", "|", "|-", "->", "Z''42'", "&"}};
  Test{{",", ",", ",", "A'228", ",", "L337"}};
  Test{{"Z''", ",", "Z'''", "|-", "Z''", "->", "Z'''"}};
  Test{{"Z'''"}};
  Test{{"Z'''", "->", "Z''", "->", "Z'''"}};
  Test{{"Z''", "->", "Z'''"}};

  // Generate variable name of specified range
  constexpr auto genVar = [] (std::size_t len) {
    assert(len > 0);
    std::string result;
    result.push_back('A' + std::uniform_int_distribution<>(0, 25)(generator));
    for (size_t i = 0; i < len; i++) {
      const auto nextChar = std::uniform_int_distribution<>(0, 40)(generator);
      if (nextChar < 26) {
        result.push_back('A' + nextChar);
      } else if (nextChar < 36) {
        result.push_back('0' + nextChar - 26);
      } else {
        result.push_back('\'');
      }
    }
    return result;
  };
  Test{{genVar(42), ",", genVar(54)}};
  Test{{genVar(42), ",", "(", genVar(54), ")", ",", genVar(228)}};
  {
    std::initializer_list<std::size_t> lengths = {2, 4, 8, 16, 31, 16, 23};
    std::vector<std::string> manyVars{lengths.size()};
    std::transform(lengths.begin(), lengths.end(), manyVars.begin(), genVar);
    Test{manyVars, /* withoutSpaces */ false};
  }
}

