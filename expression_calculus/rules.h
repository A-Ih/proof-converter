#pragma once

#include "expression.h"
#include <algorithm>
#include <memory>

enum class RuleType {
  AX,
  EIMPL,
  IIMPL,
  ICON,
  ELCON,
  ERCON,
  ILDIS,
  IRDIS,
  EDIS,
  EBOT
};

namespace Rules {

using TPtr = std::shared_ptr<Semantic::Expression>;

struct NaturalNode {
  NaturalNode(TPtr additionalHypothesis, TPtr expression) :
    addHyp{additionalHypothesis},
    expr{expression}
  {}

  TPtr addHyp;  // = nullptr if the context is the same as parents'. Else - this
                // expression should be added to the context of the parent
  TPtr expr;

  virtual RuleType GetRuleType() const = 0;

  virtual ~NaturalNode() = default;
};

struct Ax : NaturalNode {
  Ax(TPtr hyp, TPtr phi) : NaturalNode{hyp, phi} {}

  RuleType GetRuleType() const final {
    return RuleType::AX;
  }
};

struct EImpl : NaturalNode {
  EImpl(TPtr hyp, TPtr psi, std::unique_ptr<NaturalNode>&& l, std::unique_ptr<NaturalNode>&& r)
    : NaturalNode{hyp, psi}, lhs{std::move(l)}, rhs{std::move(r)} {}

  RuleType GetRuleType() const final {
    return RuleType::EIMPL;
  }

  std::unique_ptr<NaturalNode> lhs;
  std::unique_ptr<NaturalNode> rhs;
};

struct IImpl : NaturalNode {
  IImpl(TPtr hyp, TPtr phiArrowPsi, std::unique_ptr<NaturalNode>&& c)
    : NaturalNode{hyp, phiArrowPsi}, child{std::move(c)} {}

  RuleType GetRuleType() const final {
    return RuleType::IIMPL;
  }

  std::unique_ptr<NaturalNode> child;
};

struct ICon : NaturalNode {
  ICon(TPtr hyp, TPtr phiConPsi, std::unique_ptr<NaturalNode>&& l, std::unique_ptr<NaturalNode>&& r)
    : NaturalNode{hyp, phiConPsi}, lhs{std::move(l)}, rhs{std::move(r)} {}

  RuleType GetRuleType() const final {
    return RuleType::ICON;
  }

  std::unique_ptr<NaturalNode> lhs;
  std::unique_ptr<NaturalNode> rhs;
};

struct ElCon : NaturalNode {
  ElCon(TPtr hyp, TPtr phi, std::unique_ptr<NaturalNode>&& c)
    : NaturalNode{hyp, phi}, child{std::move(c)} {}

  RuleType GetRuleType() const final {
    return RuleType::ELCON;
  }

  std::unique_ptr<NaturalNode> child;
};

struct ErCon : NaturalNode {
  ErCon(TPtr hyp, TPtr phi, std::unique_ptr<NaturalNode>&& c)
    : NaturalNode{hyp, phi}, child{std::move(c)} {}

  RuleType GetRuleType() const final {
    return RuleType::ERCON;
  }

  std::unique_ptr<NaturalNode> child;
};

struct IlDis : NaturalNode {
  IlDis(TPtr hyp, TPtr phiDisPsi, std::unique_ptr<NaturalNode>&& c)
    : NaturalNode{hyp, phiDisPsi}, child{std::move(c)} {}

  RuleType GetRuleType() const final {
    return RuleType::ILDIS;
  }

  std::unique_ptr<NaturalNode> child;
};

struct IrDis : NaturalNode {
  IrDis(TPtr hyp, TPtr phiDisPsi, std::unique_ptr<NaturalNode>&& c)
    : NaturalNode{hyp, phiDisPsi}, child{std::move(c)} {}

  RuleType GetRuleType() const final {
    return RuleType::IRDIS;
  }

  std::unique_ptr<NaturalNode> child;
};

struct EDis : NaturalNode {
  EDis(TPtr hyp, TPtr rho, std::unique_ptr<NaturalNode>&& l, std::unique_ptr<NaturalNode>&& m, std::unique_ptr<NaturalNode>&& r)
    : NaturalNode{hyp, rho}, lhs{std::move(l)}, mhs{std::move(m)}, rhs{std::move(r)} {}

  RuleType GetRuleType() const final {
    return RuleType::EDIS;
  }

  std::unique_ptr<NaturalNode> lhs;
  std::unique_ptr<NaturalNode> mhs;
  std::unique_ptr<NaturalNode> rhs;
};

struct EBot : NaturalNode {
  EBot(TPtr hyp, TPtr phi) : NaturalNode{hyp, phi} {}

  RuleType GetRuleType() const final {
    return RuleType::EBOT;
  }
};

template<typename THead, typename ...TTail>
inline constexpr bool ARE_SAME_V = (std::is_same_v<THead, TTail> && ...);

inline std::unique_ptr<NaturalNode> MakeAx1(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `a -> b -> a`
  using namespace Semantic;
  auto a = GetComponent<Implication>(phi.get())->left;
  auto bArrowA = GetComponent<Implication>(phi.get())->right;
  auto b = GetComponent<Implication>(phi.get(), &Implication::right)->left;
  static_assert(ARE_SAME_V<TPtr, decltype(a), decltype(bArrowA), decltype(b)>);
  return
    std::make_unique<IImpl>(TPtr{}, phi,
      std::make_unique<IImpl>(a, bArrowA,
        std::make_unique<Ax>(b, a)));
}

inline std::unique_ptr<NaturalNode> MakeAx2(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `(a -> b) -> (a -> b -> y) -> (a -> y)`
  using namespace Semantic;
  auto ab = GetComponent<Implication>(phi.get())->left;
  auto abyAy = GetComponent<Implication>(phi.get())->right;
  auto aby = GetComponent<Implication>(abyAy.get())->left;
  auto ay = GetComponent<Implication>(abyAy.get())->right;
  auto a = GetComponent<Implication>(ab.get())->left;
  auto b = GetComponent<Implication>(ab.get())->right;
  auto y = GetComponent<Implication>(ay.get())->right;
  auto by = std::make_shared<Implication>(b, y);
  static_assert(ARE_SAME_V<TPtr, decltype(ab), decltype(abyAy), decltype(aby), decltype(ay), decltype(a), decltype(b), decltype(y)>);
  return
    std::make_unique<IImpl>(TPtr{}, phi,
        std::make_unique<IImpl>(ab, abyAy,
          std::make_unique<IImpl>(aby, ay,
            std::make_unique<EImpl>(a, y,
              std::make_unique<EImpl>(TPtr{}, by,
                std::make_unique<Ax>(TPtr{}, aby),
                std::make_unique<Ax>(TPtr{}, a)),
              std::make_unique<EImpl>(TPtr{}, b,
                std::make_unique<Ax>(TPtr{}, ab),
                std::make_unique<Ax>(TPtr{}, a))))));
}

inline std::unique_ptr<NaturalNode> MakeAx3(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `a -> b -> a & b`
  using namespace Semantic;
  auto bArrowAAndB = GetComponent<Implication>(phi.get())->right;  // b -> a & b
  auto aAndB = GetComponent<Implication>(bArrowAAndB.get())->right;  // a & b
  auto a = GetComponent<Conjunction>(aAndB.get())->left;  // a
  auto b = GetComponent<Conjunction>(aAndB.get())->right;  // b
  static_assert(ARE_SAME_V<TPtr, decltype(bArrowAAndB), decltype(aAndB), decltype(a), decltype(b)>);
  return
    std::make_unique<IImpl>(TPtr{}, phi,
        std::make_unique<IImpl>(a, bArrowAAndB,
          std::make_unique<ICon>(b, aAndB,
            std::make_unique<Ax>(TPtr{}, a),
            std::make_unique<Ax>(TPtr{}, b))));
}

inline std::unique_ptr<NaturalNode> MakeAx4(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `a & b -> a`
  using namespace Semantic;
  auto aAndB = GetComponent<Implication>(phi.get())->left;  // a & b
  auto a = GetComponent<Conjunction>(aAndB.get())->left;  // a
  auto b = GetComponent<Conjunction>(aAndB.get())->right;  // b
  static_assert(ARE_SAME_V<TPtr, decltype(aAndB), decltype(a), decltype(b)>);
  return
    std::make_unique<IImpl>(TPtr{}, phi,
        std::make_unique<ElCon>(aAndB, a,
          std::make_unique<Ax>(TPtr{}, aAndB)));
}

inline std::unique_ptr<NaturalNode> MakeAx5(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `a & b -> b`
  using namespace Semantic;
  auto aAndB = GetComponent<Implication>(phi.get())->left;  // a & b
  auto a = GetComponent<Conjunction>(aAndB.get())->left;  // a
  auto b = GetComponent<Conjunction>(aAndB.get())->right;  // b
  static_assert(ARE_SAME_V<TPtr, decltype(aAndB), decltype(a), decltype(b)>);
  return
    std::make_unique<IImpl>(TPtr{}, phi,
        std::make_unique<ErCon>(aAndB, b,
          std::make_unique<Ax>(TPtr{}, aAndB)));
}

inline std::unique_ptr<NaturalNode> MakeAx6(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `a -> a | b`
  using namespace Semantic;
  auto aOrB = GetComponent<Implication>(phi.get())->right;  // a | b
  auto a = GetComponent<Disjunction>(aOrB.get())->left;  // a
  auto b = GetComponent<Disjunction>(aOrB.get())->right;  // b
  static_assert(ARE_SAME_V<TPtr, decltype(aOrB), decltype(a), decltype(b)>);
  return
    std::make_unique<IImpl>(TPtr{}, phi,
        std::make_unique<IlDis>(a, aOrB,
          std::make_unique<Ax>(TPtr{}, a)));
}

inline std::unique_ptr<NaturalNode> MakeAx7(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `b -> a | b`
  using namespace Semantic;
  auto aOrB = GetComponent<Implication>(phi.get())->right;  // a | b
  auto a = GetComponent<Disjunction>(aOrB.get())->left;  // a
  auto b = GetComponent<Disjunction>(aOrB.get())->right;  // b
  static_assert(ARE_SAME_V<TPtr, decltype(aOrB), decltype(a), decltype(b)>);
  return
    std::make_unique<IImpl>(TPtr{}, phi,
        std::make_unique<IrDis>(b, aOrB,
          std::make_unique<Ax>(TPtr{}, a)));
}


}  // namespace Rules
