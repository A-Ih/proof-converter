#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <cassert>
#include <functional>

// No negation since it is interpreted as (a -> _|_)

enum class ExpressionType : std::size_t {
  CONJUNCTION,
  DISJUNCTION,
  IMPLICATION,
  VARIABLE,
  BOTTOM
};

namespace Regular {

struct Expression {
  virtual ExpressionType GetType() const = 0;

  virtual ~Expression() = default;
};

struct BinaryOperationBase : Expression {
  BinaryOperationBase(std::unique_ptr<Expression>&& lhs, std::unique_ptr<Expression>&& rhs) :
    left{std::move(lhs)}, right{std::move(rhs)}
  {}

  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
};

template<ExpressionType EXPRESSION_TYPE>
struct BinaryOperation : BinaryOperationBase {
  using BinaryOperationBase::BinaryOperationBase;

  static const BinaryOperation<EXPRESSION_TYPE>* fromExpression(const Expression* expr) {
    assert(expr->GetType() == EXPRESSION_TYPE);
    return static_cast<const BinaryOperation<EXPRESSION_TYPE>*>(expr);
  }

  ExpressionType GetType() const final {
    return EXPRESSION_TYPE;
  }
};

using Conjunction = BinaryOperation<ExpressionType::CONJUNCTION>;
using Disjunction = BinaryOperation<ExpressionType::DISJUNCTION>;
using Implication = BinaryOperation<ExpressionType::IMPLICATION>;


// A singleton
struct Bottom : Expression {
  static const Bottom* fromExpression(const Expression* expr) {
    assert(expr->GetType() == ExpressionType::BOTTOM);
    return static_cast<const Bottom*>(expr);
  }

  ExpressionType GetType() const final {
    return ExpressionType::BOTTOM;
  }

};

// TODO: global cache of variables (just like a singleton bottom)
struct Variable : Expression {
  Variable(std::string name) : name{name} {}

  static const Variable* fromExpression(const Expression* expr) {
    assert(expr->GetType() == ExpressionType::VARIABLE);
    return static_cast<const Variable*>(expr);
  }

  ExpressionType GetType() const final {
    return ExpressionType::VARIABLE;
  }

  std::string name;
};

template<typename TComp>
const TComp* GetComponent(const Expression* expr) {
  return dynamic_cast<const TComp*>(expr);
}

template<typename TComp, typename TExpr, typename TField, typename ...TRest, typename ...TFields>
const TComp* GetComponent(const Expression* expr, TField TExpr::* fieldPtr, TFields TRest::*... rest) {
  auto downcastedExpr = dynamic_cast<const TExpr*>(expr);
  if (downcastedExpr == nullptr) {
    return nullptr;
  }
  if constexpr (std::is_same_v<TField, std::unique_ptr<Expression>>) {
    return GetComponent<TComp>((downcastedExpr->*fieldPtr).get(), rest...);
  } else if constexpr (std::is_same_v<TComp, TField>) {
    return &(downcastedExpr->*fieldPtr);
  } else {
    // The access is invalid
    return nullptr;
  }
}

std::string ToString(const Expression* expr);

}  // namespace Regular

namespace Semantic {

struct Expression : Regular::Expression {
  Expression(std::string_view view) :
    expressionView{view},
    memoizedHash{std::hash<std::string_view>{}(view)}
  {}

  std::size_t Len() const {
    return expressionView.size();
  }

  std::string_view GetView() const {
    return expressionView;
  }

  virtual ~Expression() = default;

  std::string_view expressionView;
  std::size_t memoizedHash;
};

struct Bottom : Expression {
  Bottom(std::string_view view) : Expression{view} {}

  ExpressionType GetType() const final {
    return ExpressionType::BOTTOM;
  }
};

struct Variable : Expression {
  Variable(std::string_view view) : Expression{view} {
    // TODO: assert that the view is a valid variable name
  }

  std::string_view GetName() {
    return this->expressionView;
  }

  ExpressionType GetType() const final {
    return ExpressionType::VARIABLE;
  }
};

template<ExpressionType EXPRESSION_TYPE>
struct BinaryOperation : Expression {
  BinaryOperation(std::string_view view, const std::shared_ptr<Expression>& lhs, const std::shared_ptr<Expression>& rhs) :
    Expression{view},
    left{lhs},
    right{rhs}
  {}

  ExpressionType GetType() const final {
    return EXPRESSION_TYPE;
  }

  std::shared_ptr<Expression> left;
  std::shared_ptr<Expression> right;
};

using Conjunction = BinaryOperation<ExpressionType::CONJUNCTION>;
using Disjunction = BinaryOperation<ExpressionType::DISJUNCTION>;
using Implication = BinaryOperation<ExpressionType::IMPLICATION>;

struct OwningExpression {
  OwningExpression(const Regular::Expression* expr);

  OwningExpression& operator=(const OwningExpression&) = delete;
  OwningExpression& operator=(OwningExpression&&) = delete;

  ~OwningExpression() {
    // The default destruction order is correct though I wouldn't rely on it.
    // The reason for that is AST holding string_views refering to
    // `expressionString` i.e. the refered string should remain intact until the
    // refering views are destroyed (since std::basic_string_view is not
    // trivially destructible (at least in c++17))
    root.reset();
  }

  std::string expressionString;
  std::shared_ptr<Expression> root;
};

bool operator==(const Expression& lhs, const Expression& rhs);

bool operator==(const OwningExpression& lhs, const OwningExpression& rhs);

}  // namespace Semantic

