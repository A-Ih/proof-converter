#include "expression_calculus/expression.h"
#include "expression_calculus/parsing.h"
#include "expression_calculus/rules.h"

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

struct Hasher {
  std::size_t operator()(const Semantic::Expression& expr) const {
    return expr.memoizedHash;
  }

  std::size_t operator()(const Semantic::OwningExpression& expr) const {
    return expr.root->memoizedHash;
  }
};

template<typename TValue>
using TMap = std::unordered_map<std::shared_ptr<Semantic::Expression>, TValue, Hasher>;

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::unordered_set<std::unique_ptr<Semantic::OwningExpression>, Hasher> hypotheses;
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
        hypotheses.insert(parser->ParseOwningExpression());
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

  const std::size_t N = proof.size();

  TMap<std::shared_ptr<Rules::NaturalNode>> precalcMP;  // Precalculated
                                                        // expressions that can
                                                        // be proven via Modus
                                                        // Ponens
  TMap<std::shared_ptr<Rules::NaturalNode>> encountered;  // Expressions that
                                                          // were already
                                                          // encountered and
                                                          // proved
  TMap<std::vector<std::size_t>> inNeedOfLhs;  // Map of following format:
                                               // a -> {a -> b_1, ..., a -> b_m)
                                               // (the stored list actually
                                               // contains the indices in
                                               // `proof` vector)
  for (std::size_t i = 0; i < N; i++) {
    if (auto prec = precalcMP.find(proof[i]->root); prec != precalcMP.end()) {
      // 1. Check if this is modus ponens
      encountered[prec->first] = prec->second;
    } else if (hypotheses.find(proof[i]) != hypotheses.end()) {
      // 2. Check if the expression is in hypotheses
      encountered[proof[i]->root] = std::make_shared<Rules::Ax>(Rules::TPtr{}, proof[i]->root);
    } else {
      // 3. Try to match to axioms (TODO)

      std::shared_ptr<Rules::NaturalNode> result;
      // Ax1: a -> b -> a
      std::cout << "Proof is incorrect at line " << i + 1 << std::endl;
      return 0;
    }

    // 4. Modus Ponens precalc (the tree for proof[i] should be present at this stage)
    if (auto impl = Semantic::GetComponent<Semantic::Implication>(proof[i]->root.get())) {
      // here we already need proof for
      auto a = impl->left;
      auto b = impl->right;
      if (auto enc = encountered.find(a); enc != encountered.end()) {
        precalcMP[b] = std::make_shared<Rules::EImpl>(Rules::TPtr{}, b, encountered[proof[i]->root], encountered[a]);
      }
    }

  }

}
