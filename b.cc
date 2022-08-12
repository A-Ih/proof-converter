#include "expression_calculus/expression.h"
#include "expression_calculus/parsing.h"
#include "expression_calculus/rules.h"

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

struct Hasher {
  std::size_t operator()(const std::shared_ptr<Semantic::Expression>& expr) const {
    assert(expr.use_count() > 0);
    return expr->memoizedHash;
  }

  std::size_t operator()(const std::shared_ptr<Semantic::OwningExpression>& expr) const {
    assert(expr.use_count() > 0);
    return expr->root->memoizedHash;
  }
};

struct ProperSharedPtrComparator {
  using T = Semantic::Expression;
  bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const {
    assert(lhs.use_count() > 0);
    assert(rhs.use_count() > 0);
    return *lhs == *rhs;
  }
};

template<typename TValue>
using TMap = std::unordered_map<std::shared_ptr<Semantic::Expression>, TValue, Hasher, ProperSharedPtrComparator>;

using TSet = std::unordered_set<std::shared_ptr<Semantic::Expression>, Hasher, ProperSharedPtrComparator>;

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::vector<std::unique_ptr<Semantic::OwningExpression>> owningHypothesesList;
  TSet hypotheses;
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
        owningHypothesesList.emplace_back(parser->ParseOwningExpression());
        hypotheses.insert(owningHypothesesList.back()->root);
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
    auto pi = proof[i]->root;
    if (auto prec = precalcMP.find(pi); prec != precalcMP.end()) {
      // 1. Check if this is modus ponens
      encountered[prec->first] = prec->second;
    } else if (hypotheses.find(pi) != hypotheses.end()) {
      // 2. Check if the expression is in hypotheses
      encountered[pi] = std::make_shared<Rules::Ax>(Rules::TPtr{}, pi);

      // 3. Try to match to axioms
    } else if (Rules::MatchAx1(pi.get())) {
      encountered[pi] = Rules::MakeAx1(pi);
    } else if (Rules::MatchAx2(pi.get())) {
      encountered[pi] = Rules::MakeAx2(pi);
    } else if (Rules::MatchAx3(pi.get())) {
      encountered[pi] = Rules::MakeAx3(pi);
    } else if (Rules::MatchAx4(pi.get())) {
      encountered[pi] = Rules::MakeAx4(pi);
    } else if (Rules::MatchAx5(pi.get())) {
      encountered[pi] = Rules::MakeAx5(pi);
    } else if (Rules::MatchAx6(pi.get())) {
      encountered[pi] = Rules::MakeAx6(pi);
    } else if (Rules::MatchAx7(pi.get())) {
      encountered[pi] = Rules::MakeAx7(pi);
    } else if (Rules::MatchAx8(pi.get())) {
      encountered[pi] = Rules::MakeAx8(pi);
    } else if (Rules::MatchAx9(pi.get())) {
      encountered[pi] = Rules::MakeAx9(pi);
    } else if (Rules::MatchAx10(pi.get())) {
      encountered[pi] = Rules::MakeAx10(pi);
    } else {
      std::cout << "Proof is incorrect at line " << i + 1 << std::endl;
      return 0;
    }

    // 4. Modus Ponens precalc (the tree for proof[i] should be present at this stage)
    if (auto impl = Semantic::GetComponent<Semantic::Implication>(pi.get())) {
      // here we already need proof for
      auto a = impl->left;
      auto b = impl->right;
      if (auto enc = encountered.find(a); enc != encountered.end()) {
        precalcMP[b] = std::make_shared<Rules::EImpl>(Rules::TPtr{}, b, encountered[pi], encountered[a]);
      } else {
        inNeedOfLhs[a].push_back(i);
      }
    }

    // 5. Second stage of modus pones precalc (clean up inNeedOfLhs)
    if (auto it = inNeedOfLhs.find(pi); it != inNeedOfLhs.end()) {
      for (auto j : it->second) {
        auto pj = proof[j]->root;
        auto bj = Semantic::GetComponent<Semantic::Implication>(pj.get())->right;
        precalcMP[bj] = std::make_shared<Rules::EImpl>(Rules::TPtr{}, bj, encountered[pj], encountered[pi]);
      }
    }
  }

}
