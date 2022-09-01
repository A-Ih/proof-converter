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
  constexpr auto IL = &Implication::left;
  constexpr auto IR = &Implication::right;
  constexpr auto DL = &Disjunction::left;
  constexpr auto DR = &Disjunction::right;
  constexpr auto CL = &Conjunction::left;
  constexpr auto CR = &Conjunction::right;
  {
    Test t{"A"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), &Variable::name), "A");
  }

  {
    Test t{"A->B"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IL, &Variable::name), "A");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, &Variable::name), "B");
  }

  {
    Test t{"A->B->C"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IL, &Variable::name), "A");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, IL, &Variable::name), "B");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, IR, &Variable::name), "C");
  }

  {
    Test t{"A->B|C->D"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IL, &Variable::name), "A");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, IL, DL, &Variable::name), "B");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, IL, DR, &Variable::name), "C");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, IR, &Variable::name), "D");
  }

  {
    Test t{"A|B->C&D"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IL, DL, &Variable::name), "A");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IL, DR, &Variable::name), "B");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, CL, &Variable::name), "C");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, CR, &Variable::name), "D");
  }

  {
    Test t{"((A))->(B|(C|D))"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IL, &Variable::name), "A");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, DL, &Variable::name), "B");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, DR, DL, &Variable::name), "C");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, DR, DR, &Variable::name), "D");
  }
  {
    Test t{"(A->B)->(A->B->C)->(A->C)"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IL, IL, &Variable::name), "A");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IL, IR, &Variable::name), "B");

    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, IL, IL, &Variable::name), "A");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, IL, IR, IL,  &Variable::name), "B");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, IL, IR, IR,  &Variable::name), "C");

    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, IR, IL, &Variable::name), "A");
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IR, IR, IR, &Variable::name), "C");
  }
  {
    Test t{"A -> B -> A & B"};
  }
  {
    Test t{"!A"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IL, &Variable::name), "A");
  }
  {
    Test t{"!((A))"};
    ASSERT_EQUAL(GetComponent<std::string>(t.GetExpr(), IL, &Variable::name), "A");
  }
}
