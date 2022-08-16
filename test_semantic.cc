#include "expression_calculus/expression.h"
#include "expression_calculus/parsing.h"

#include <iostream>
#include <cstdlib>

// TODO: variadic getter that returns optional (or throws an exception idk, the
// error messages should be easily diagnosible)

#define ASSERT_EQUAL(x, y) \
  if (x != y) { std::cerr << "Failed condition " #x " == " #y << std::endl; std::abort(); }

struct Test {
public:
  Test(std::string exprStr) {
    std::cout << "Testing '" << exprStr << "'..." << std::flush;
    auto parser = std::make_unique<Parser>(exprStr);
    expr = parser->ParseExpression();
    owningExpr = std::make_unique<Semantic::OwningExpression>(expr.get());
  }

  const Regular::Expression* GetExpr() const {
    return expr.get();
  }

  const std::string_view GetPrefixView() const {
    return owningExpr->expressionString;
  }

  ~Test() {
    std::cout << "Done" << std::endl;
  }

private:
  std::unique_ptr<Regular::Expression> expr;
  std::unique_ptr<Semantic::OwningExpression> owningExpr;
};

int main() {
  {
    Test t{"A"};
    ASSERT_EQUAL(t.GetPrefixView(), "A");
  }

  {
    Test t{"A->B"};
    ASSERT_EQUAL(t.GetPrefixView(), "-> A B");
  }

  {
    Test t{"A->B->C"};
    ASSERT_EQUAL(t.GetPrefixView(), "-> A -> B C")
  }

  {
    Test t{"A->B|C->D"};
    ASSERT_EQUAL(t.GetPrefixView(), "-> A -> | B C D")
  }

  {
    Test t{"A|B->C&D"};
    ASSERT_EQUAL(t.GetPrefixView(), "-> | A B & C D");
  }

  {
    Test t{"((A))->(B|(C|D))"};
    ASSERT_EQUAL(t.GetPrefixView(), "-> A | B | C D");
  }

  {
    Test t{"A -> B -> A & B"};
    ASSERT_EQUAL(t.GetPrefixView(), "-> A -> B & A B");
  }

  {
    Test t{"(A->B)->(A->B->C)->(A->C)"};
    ASSERT_EQUAL(t.GetPrefixView(), "-> -> A B -> -> A -> B C -> A C");
  }
}

