#include "expression_calculus/parsing.h"

#include <iostream>
#include <string>
#include <memory>

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::vector<std::unique_ptr<Semantic::OwningExpression>> hypotheses;
  std::unique_ptr<Semantic::OwningExpression> provenExpression;
  std::vector<std::unique_ptr<Semantic::OwningExpression>> proof;

  {
    std::string firstLine;
    std::getline(std::cin, firstLine);
    auto parser = std::make_unique<Parser>(firstLine);

    if (parser->ParseToken(TokenType::TURNSTILE)) {
      provenExpression = parser->ParseOwningExpression();
    } else {
      do {
        hypotheses.emplace_back(parser->ParseOwningExpression());
      } while (parser->ParseToken(TokenType::COMMA));
      if (!parser->ParseToken(TokenType::TURNSTILE)) {
        std::cerr << "Turnstile expected, '" << parser->PeekToken() << "'" << std::endl;
        return 1;
      }
      provenExpression = parser->ParseOwningExpression();
    }
    assert(parser->IsExhausted());
  }

  for (std::string proofLine; std::getline(std::cin, proofLine) && std::cin.good();) {
    auto parser = std::make_unique<Parser>(proofLine);
    proof.emplace_back(parser->ParseOwningExpression());
    assert(parser->IsExhausted());
  }

  if (!(*proof.back() == *provenExpression)) {
    std::cout << "The proof does not prove the required expression" << std::endl;
    return 0;
  }
}
