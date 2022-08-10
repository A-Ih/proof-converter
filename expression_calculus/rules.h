#pragma once

#include "expression.h"
#include <algorithm>

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
  Ax(TPtr phi) : NaturalNode{{}, phi} {}

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

inline std::unique_ptr<NaturalNode> MakeAx1(
    std::shared_ptr<Semantic::Expression> phi,
    std::shared_ptr<Semantic::Expression> a,
    std::shared_ptr<Semantic::Expression> b) {
  // TODO: GetComponent
  // Although we can get away without GetComponent and just construct the shit
  // (this yields more wasted resources)
  return {};

}

}  // namespace Rules
