#include "rules.h"

namespace Rules {
/*******************************************************************************
*                               Axiom matching                                *
*******************************************************************************/

bool MatchAx1(const Semantic::Expression* expr) {
  // a1 -> b -> a2
  using namespace Semantic;
  auto a1 = GetComponent<Expression>(expr, &Implication::left);
  auto a2 = GetComponent<Expression>(expr, &Implication::right, &Implication::right);
  return a1 && a2 && *a1 == *a2;
}

bool MatchAx2(const Semantic::Expression* expr) {
  // (a1 -> b1) -> (a2 -> b2 -> y1) -> (a3 -> y2)
  using namespace Semantic;
  constexpr auto l = &Implication::left;
  constexpr auto r = &Implication::right;
  auto a1 = GetComponent<Expression>(expr, l, l);
  auto b1 = GetComponent<Expression>(expr, l, r);

  auto a2 = GetComponent<Expression>(expr, r, l, l);
  auto b2 = GetComponent<Expression>(expr, r, l, r, l);
  auto y1 = GetComponent<Expression>(expr, r, l, r, r);

  auto a3 = GetComponent<Expression>(expr, r, r, l);
  auto y2 = GetComponent<Expression>(expr, r, r, r);
  return a1 && a2 && a3 && b1 && b2 && y1 && y2
    && *a1 == *a2 && *a2 == *a3
    && *b1 == *b2
    && *y1 == *y2;
}

bool MatchAx3(const Semantic::Expression* expr) {
  // a1 -> b1 -> a2 & b2
  using namespace Semantic;
  auto a1 = GetComponent<Expression>(expr, &Implication::left);
  auto b1 = GetComponent<Expression>(expr, &Implication::right, &Implication::left);

  auto a2 = GetComponent<Expression>(expr, &Implication::right, &Implication::right, &Conjunction::left);
  auto b2 = GetComponent<Expression>(expr, &Implication::right, &Implication::right, &Conjunction::right);
  return a1 && a2 && b1 && b2
    && *a1 == *a2
    && *b1 == *b2;
}

bool MatchAx4(const Semantic::Expression* expr) {
  // a1 && b -> a2
  using namespace Semantic;
  auto a1 = GetComponent<Expression>(expr, &Implication::left, &Conjunction::left);
  auto a2 = GetComponent<Expression>(expr, &Implication::right);
  return a1 && a2 && *a1 == *a2;
}

bool MatchAx5(const Semantic::Expression* expr) {
  // a && b1 -> b2
  using namespace Semantic;
  auto b1 = GetComponent<Expression>(expr, &Implication::left, &Conjunction::right);
  auto b2 = GetComponent<Expression>(expr, &Implication::right);
  return b1 && b2 && *b1 == *b2;
}

bool MatchAx6(const Semantic::Expression* expr) {
  // a1 -> a2 | b
  using namespace Semantic;
  auto a1 = GetComponent<Expression>(expr, &Implication::left);
  auto a2 = GetComponent<Expression>(expr, &Implication::right, &Disjunction::left);
  return a1 && a2 && *a1 == *a2;
}

bool MatchAx7(const Semantic::Expression* expr) {
  // b1 -> a | b2
  using namespace Semantic;
  auto b1 = GetComponent<Expression>(expr, &Implication::left);
  auto b2 = GetComponent<Expression>(expr, &Implication::right, &Disjunction::right);
  return b1 && b2 && *b1 == *b2;
}

bool MatchAx8(const Semantic::Expression* expr) {
  // (a1 -> y1) -> (b1 -> y2) -> (a2 | b2 -> y3)
  using namespace Semantic;
  constexpr auto il = &Implication::left;
  constexpr auto ir = &Implication::right;
  constexpr auto dl = &Disjunction::left;
  constexpr auto dr = &Disjunction::right;
  auto a1 = GetComponent<Expression>(expr, il, il);
  auto y1 = GetComponent<Expression>(expr, il, ir);

  auto b1 = GetComponent<Expression>(expr, ir, il, il);
  auto y2 = GetComponent<Expression>(expr, ir, il, ir);

  auto a2 = GetComponent<Expression>(expr, ir, ir, il, dl);
  auto b2 = GetComponent<Expression>(expr, ir, ir, il, dr);
  auto y3 = GetComponent<Expression>(expr, ir, ir, ir);

  return a1 && a2 && b1 && b2 && y1 && y2 && y3
    && *a1 == *a2
    && *b1 == *b2
    && *y1 == *y2 && *y2 == *y3;
}

bool MatchAx9(const Semantic::Expression* expr) {
  // (a1 -> b1) -> (a2 -> b2 -> _|_1) -> (a3 -> _|_2)
  using namespace Semantic;
  constexpr auto l = &Implication::left;
  constexpr auto r = &Implication::right;
  auto a1 = GetComponent<Expression>(expr, l, l);
  auto b1 = GetComponent<Expression>(expr, l, r);

  auto a2 = GetComponent<Expression>(expr, r, l, l);
  auto b2 = GetComponent<Expression>(expr, r, l, r, l);
  auto bot1 = GetComponent<Bottom>(expr, r, l, r, r);

  auto a3 = GetComponent<Expression>(expr, r, r, l);
  auto bot2 = GetComponent<Bottom>(expr, r, r, r);

  return a1 && a2 && a3 && b1 && b2 && bot1 && bot2
    && *a1 == *a2 && *a2 == *a3
    && *b1 == *b2
    && *bot1 == *bot2;
}

bool MatchAx10(const Semantic::Expression* expr) {
  // a1 -> (a2 -> bot) -> b
  using namespace Semantic;

  auto a1 = Semantic::GetComponent<Expression>(expr, &Implication::left);
  auto a2 = Semantic::GetComponent<Expression>(expr, &Implication::right, &Implication::left, &Implication::left);
  auto bot = Semantic::GetComponent<Bottom>(expr, &Implication::right, &Implication::left, &Implication::right);

  return a1 && a2 && bot && *a1 == *a2;
}

/*******************************************************************************
*                             Axiom tree building                             *
*******************************************************************************/

std::shared_ptr<NaturalNode> MakeAx1(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `a -> b -> a`
  using namespace Semantic;
  auto a = GetComponent<Implication>(phi.get())->left;
  auto bArrowA = GetComponent<Implication>(phi.get())->right;
  auto b = GetComponent<Implication>(phi.get(), &Implication::right)->left;
  return
    std::make_shared<IImpl>(TPtr{}, phi,
      std::make_shared<IImpl>(a, bArrowA,
        std::make_shared<Ax>(b, a)));
}

std::shared_ptr<NaturalNode> MakeAx2(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `(a -> b) -> (a -> b -> y) -> (a -> y)`
  using namespace Semantic;
  auto ab = GetComponent<Implication>(phi.get())->left;
  auto abyAy = GetComponent<Implication>(phi.get())->right;
  auto aby = GetComponent<Implication>(abyAy.get())->left;
  auto ay = GetComponent<Implication>(abyAy.get())->right;
  auto a = GetComponent<Implication>(ab.get())->left;
  auto b = GetComponent<Implication>(ab.get())->right;
  auto y = GetComponent<Implication>(ay.get())->right;
  auto by = GetComponent<Implication>(aby.get())->right;
  return
    std::make_shared<IImpl>(TPtr{}, phi,
        std::make_shared<IImpl>(ab, abyAy,
          std::make_shared<IImpl>(aby, ay,
            std::make_shared<EImpl>(a, y,
              std::make_shared<EImpl>(TPtr{}, by,
                std::make_shared<Ax>(TPtr{}, aby),
                std::make_shared<Ax>(TPtr{}, a)),
              std::make_shared<EImpl>(TPtr{}, b,
                std::make_shared<Ax>(TPtr{}, ab),
                std::make_shared<Ax>(TPtr{}, a))))));
}

std::shared_ptr<NaturalNode> MakeAx3(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `a -> b -> a & b`
  using namespace Semantic;
  auto bArrowAAndB = GetComponent<Implication>(phi.get())->right;  // b -> a & b
  auto aAndB = GetComponent<Implication>(bArrowAAndB.get())->right;  // a & b
  auto a = GetComponent<Conjunction>(aAndB.get())->left;  // a
  auto b = GetComponent<Conjunction>(aAndB.get())->right;  // b
  return
    std::make_shared<IImpl>(TPtr{}, phi,
        std::make_shared<IImpl>(a, bArrowAAndB,
          std::make_shared<ICon>(b, aAndB,
            std::make_shared<Ax>(TPtr{}, a),
            std::make_shared<Ax>(TPtr{}, b))));
}

std::shared_ptr<NaturalNode> MakeAx4(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `a & b -> a`
  using namespace Semantic;
  auto aAndB = GetComponent<Implication>(phi.get())->left;  // a & b
  auto a = GetComponent<Conjunction>(aAndB.get())->left;  // a
  auto b = GetComponent<Conjunction>(aAndB.get())->right;  // b
  return
    std::make_shared<IImpl>(TPtr{}, phi,
        std::make_shared<ElCon>(aAndB, a,
          std::make_shared<Ax>(TPtr{}, aAndB)));
}

std::shared_ptr<NaturalNode> MakeAx5(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `a & b -> b`
  using namespace Semantic;
  auto aAndB = GetComponent<Implication>(phi.get())->left;  // a & b
  auto a = GetComponent<Conjunction>(aAndB.get())->left;  // a
  auto b = GetComponent<Conjunction>(aAndB.get())->right;  // b
  return
    std::make_shared<IImpl>(TPtr{}, phi,
        std::make_shared<ErCon>(aAndB, b,
          std::make_shared<Ax>(TPtr{}, aAndB)));
}

std::shared_ptr<NaturalNode> MakeAx6(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `a -> a | b`
  using namespace Semantic;
  auto aOrB = GetComponent<Implication>(phi.get())->right;  // a | b
  auto a = GetComponent<Disjunction>(aOrB.get())->left;  // a
  auto b = GetComponent<Disjunction>(aOrB.get())->right;  // b
  return
    std::make_shared<IImpl>(TPtr{}, phi,
        std::make_shared<IlDis>(a, aOrB,
          std::make_shared<Ax>(TPtr{}, a)));
}

std::shared_ptr<NaturalNode> MakeAx7(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `b -> a | b`
  using namespace Semantic;
  auto aOrB = GetComponent<Implication>(phi.get())->right;  // a | b
  auto a = GetComponent<Disjunction>(aOrB.get())->left;  // a
  auto b = GetComponent<Disjunction>(aOrB.get())->right;  // b
  return
    std::make_shared<IImpl>(TPtr{}, phi,
        std::make_shared<IrDis>(b, aOrB,
          std::make_shared<Ax>(TPtr{}, b)));
}

std::shared_ptr<NaturalNode> MakeAx8(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `(a -> y) -> (b -> y) -> (a | b -> y)`
  using namespace Semantic;
  auto ay = GetComponent<Implication>(phi.get())->left;  // a -> b
  auto byaby = GetComponent<Implication>(phi.get())->right;  // (b -> y) -> (a | b -> y)
  auto by = GetComponent<Implication>(byaby.get())->left;  // b -> y
  auto aby = GetComponent<Implication>(byaby.get())->right;  // a | b -> y
  auto ab = GetComponent<Implication>(aby.get())->left;  // a | b
  auto a = GetComponent<Disjunction>(ab.get())->left;  // a
  auto b = GetComponent<Disjunction>(ab.get())->right;  // b
  auto y = GetComponent<Implication>(aby.get())->right;  // y
  return
    std::make_shared<IImpl>(TPtr{}, phi,
        std::make_shared<IImpl>(ay, byaby,
          std::make_shared<IImpl>(by, aby,
            std::make_shared<EDis>(ab, y,
              std::make_shared<EImpl>(a, y,
                std::make_shared<Ax>(TPtr{}, ay),
                std::make_shared<Ax>(TPtr{}, a)),
              std::make_shared<EImpl>(b, y,
                std::make_shared<Ax>(TPtr{}, by),
                std::make_shared<Ax>(TPtr{}, b)),
              std::make_shared<Ax>(TPtr{}, ab)))));
}

std::shared_ptr<NaturalNode> MakeAx9(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `(a -> b) -> (a -> b -> _|_) -> (a -> _|_)`
  using namespace Semantic;
  auto ab = GetComponent<Implication>(phi.get())->left;  // a -> b
  auto ab_a_ = GetComponent<Implication>(phi.get())->right;  // (a -> b -> _|_) -> (a -> _|_)
  auto ab_ = GetComponent<Implication>(ab_a_.get())->left;  // a -> b -> _|_
  auto a_ = GetComponent<Implication>(ab_a_.get())->right;  // a -> _|_
  auto b_ = GetComponent<Implication>(ab_.get())->right;  // b -> _|_
  auto a = GetComponent<Implication>(ab.get())->left;  // a
  auto b = GetComponent<Implication>(ab.get())->right;  // b
  auto bot = GetComponent<Implication>(a_.get())->right;  // _|_
  return
    std::make_shared<IImpl>(TPtr{}, phi,
        std::make_shared<IImpl>(ab, ab_a_,
          std::make_shared<IImpl>(ab_, a_,
            std::make_shared<EImpl>(a, bot,
              std::make_shared<EImpl>(TPtr{}, b_,
                std::make_shared<Ax>(TPtr{}, ab_),
                std::make_shared<Ax>(TPtr{}, a)),
              std::make_shared<EImpl>(TPtr{}, b,
                std::make_shared<Ax>(TPtr{}, ab),
                std::make_shared<Ax>(TPtr{}, a))))));
}

std::shared_ptr<NaturalNode> MakeAx10(std::shared_ptr<Semantic::Expression> phi) {
  // Precondition: phi has a structure like `a -> (a -> _|_) -> b`
  using namespace Semantic;
  auto a_b = GetComponent<Implication>(phi.get())->right;  // (a -> _|_) -> b
  auto a = GetComponent<Implication>(phi.get())->left;  // a
  auto b = GetComponent<Implication>(a_b.get())->right;  // b
  auto a_ = GetComponent<Implication>(a_b.get())->left;  // a -> _|_
  auto bot = GetComponent<Implication>(a_.get())->right;  // _|_
  auto _b = std::make_shared<Implication>(std::string_view{}, bot, b); // _|_ -> b
  //        v---------------------------------------^
  // this stuff is super sketchy!!! We violate the invariant but it's reasonable
  // because we won't calculate hash and compare expressions inside NaturalNode
  // (this AST serves only as an intermediate representation)
  return
    std::make_shared<IImpl>(TPtr{}, phi,
        std::make_shared<IImpl>(a, a_b,
          std::make_shared<EImpl>(a_, b,
            std::make_shared<IImpl>(TPtr{}, _b,
              std::make_shared<EBot>(bot, b,
                std::make_shared<Ax>(TPtr{}, bot))),
            std::make_shared<EImpl>(TPtr{}, bot,
              std::make_shared<Ax>(TPtr{}, a_),
              std::make_shared<Ax>(TPtr{}, a)))));
}


}  // namespace Rules

