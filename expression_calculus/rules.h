#pragma once

#include "expression.h"
#include <algorithm>
#include <memory>

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

  virtual std::string_view GetAnnotation() const = 0;
  virtual std::vector<std::shared_ptr<NaturalNode>> GetChildren() const = 0;

  virtual ~NaturalNode() = default;
};

template<std::size_t Children, const char* Annotation>
struct GenericNode : NaturalNode {
  template<typename... TChildren, typename = std::enable_if_t<Children == sizeof...(TChildren)>>
  GenericNode(TPtr hyp, TPtr psi, TChildren&&... c) : NaturalNode{hyp, psi}, children{std::forward<TChildren>(c)...} {}

  virtual std::string_view GetAnnotation() const final {
    return Annotation;
  }

  virtual std::vector<std::shared_ptr<NaturalNode>> GetChildren() const final {
    return std::vector<std::shared_ptr<NaturalNode>>(children.begin(), children.end());
  }

  std::array<std::shared_ptr<NaturalNode>, Children> children;
};

namespace Detail {
  static const char Ax[] = "Ax";
  static const char EImpl[] = "E->";
  static const char IImpl[] = "I->";
  static const char ICon[] = "I&";
  static const char ElCon[] = "El&";
  static const char ErCon[] = "Er&";
  static const char IlDis[] = "Il|";
  static const char IrDis[] = "Ir|";
  static const char EDis[] = "E|";
  static const char EBot[] = "E_|_";
}

using Ax = GenericNode<0, Detail::Ax>;
using EImpl = GenericNode<2, Detail::EImpl>;
using IImpl = GenericNode<1, Detail::IImpl>;
using ICon = GenericNode<2, Detail::ICon>;
using ElCon = GenericNode<1, Detail::ElCon>;
using ErCon = GenericNode<1, Detail::ErCon>;
using IlDis = GenericNode<1, Detail::IlDis>;
using IrDis = GenericNode<1, Detail::IrDis>;
using EDis = GenericNode<3, Detail::EDis>;
using EBot = GenericNode<1, Detail::EBot>;

/*******************************************************************************
*                               Axiom matching                                *
*******************************************************************************/

bool MatchAx1(const Semantic::Expression* expr);

bool MatchAx2(const Semantic::Expression* expr);

bool MatchAx3(const Semantic::Expression* expr);

bool MatchAx4(const Semantic::Expression* expr);

bool MatchAx5(const Semantic::Expression* expr);

bool MatchAx6(const Semantic::Expression* expr);

bool MatchAx7(const Semantic::Expression* expr);

bool MatchAx8(const Semantic::Expression* expr);

bool MatchAx9(const Semantic::Expression* expr);

bool MatchAx10(const Semantic::Expression* expr);

/*******************************************************************************
*                             Axiom tree building                             *
*******************************************************************************/

std::shared_ptr<NaturalNode> MakeAx1(std::shared_ptr<Semantic::Expression> phi);

std::shared_ptr<NaturalNode> MakeAx2(std::shared_ptr<Semantic::Expression> phi);

std::shared_ptr<NaturalNode> MakeAx3(std::shared_ptr<Semantic::Expression> phi);

std::shared_ptr<NaturalNode> MakeAx4(std::shared_ptr<Semantic::Expression> phi);

std::shared_ptr<NaturalNode> MakeAx5(std::shared_ptr<Semantic::Expression> phi);

std::shared_ptr<NaturalNode> MakeAx6(std::shared_ptr<Semantic::Expression> phi);

std::shared_ptr<NaturalNode> MakeAx7(std::shared_ptr<Semantic::Expression> phi);

std::shared_ptr<NaturalNode> MakeAx8(std::shared_ptr<Semantic::Expression> phi);

std::shared_ptr<NaturalNode> MakeAx9(std::shared_ptr<Semantic::Expression> phi);

std::shared_ptr<NaturalNode> MakeAx10(std::shared_ptr<Semantic::Expression> phi);

}  // namespace Rules
