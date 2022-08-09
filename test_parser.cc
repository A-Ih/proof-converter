#include "expression_calculus/expression.h"
#include "expression_calculus/parsing.h"

#include <iostream>
#include <cstdlib>

// TODO: variadic getter that returns optional (or throws an exception idk, the
// error messages should be easily diagnosible)

#define ASSERT_EQUAL(x, y) \
  { \
  auto v = x; \
  if (x == nullptr) { std::cerr << #x " is nullptr" << std::endl; std::abort(); } \
  if (*v != (y)) { std::cerr << "Failed condition *" #x " == " #y << std::endl; std::abort(); } \
  }

struct Test {
public:
  Test(std::string exprStr) {
    std::cout << "Testing '" << exprStr << "'..." << std::flush;
    auto parser = std::make_unique<Parser>(exprStr);
    expr = parser->ParseExpression();
  }

  const Regular::Expression* GetExpr() const {
    return expr.get();
  }

  ~Test() {
    std::cout << "Done" << std::endl;
  }

private:
  std::unique_ptr<Regular::Expression> expr;
};

int main() {
  using namespace Regular;
  {
    Test t{"A"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Variable::name), "A");
  }

  {
    Test t{"A->B"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::left, &Variable::name), "A");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::right, &Variable::name), "B");
  }

  {
    Test t{"A->B->C"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::left, &Variable::name), "A");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::right, &Implication::left, &Variable::name), "B");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::right, &Implication::right, &Variable::name), "C");
  }

  {
    Test t{"A->B|C->D"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::left, &Variable::name), "A");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::right, &Implication::left, &Disjunction::left, &Variable::name), "B");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::right, &Implication::left, &Disjunction::right, &Variable::name), "C");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::right, &Implication::right, &Variable::name), "D");
  }

  {
    Test t{"A|B->C&D"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::left, &Disjunction::left, &Variable::name), "A");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::left, &Disjunction::right, &Variable::name), "B");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::right, &Conjunction::left, &Variable::name), "C");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::right, &Conjunction::right, &Variable::name), "D");
  }

  {
    Test t{"((A))->(B|(C|D))"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::left, &Variable::name), "A");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::right, &Disjunction::left, &Variable::name), "B");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::right, &Disjunction::right, &Disjunction::left, &Variable::name), "C");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Implication::right, &Disjunction::right, &Disjunction::right, &Variable::name), "D");
  }

  // TODO: test negation
}
