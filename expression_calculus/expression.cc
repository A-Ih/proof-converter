#include "expression.h"
#include <iostream>

namespace {

  // TODO: it seems a lot easier to build the second AST as we are building this
  // massive string (it is easier to detect the boundaries of expressions).
  // However there are issues with lifetime, so it would be better to just save
  // indexes
  // The start of expression
void RegularToPrefixNotation(const Regular::Expression* expr, std::string& result) {
  switch (expr->GetType()) {
    case ExpressionType::BOTTOM:
      result.append("_|_");
      break;
    case ExpressionType::VARIABLE:
      result.append(Regular::Variable::fromExpression(expr)->name);
      break;
    case ExpressionType::CONJUNCTION:
      result.append("& ");
      RegularToPrefixNotation(Regular::BinaryOperation<ExpressionType::CONJUNCTION>::fromExpression(expr)->left.get(), result);
      result.append(" ");
      RegularToPrefixNotation(Regular::BinaryOperation<ExpressionType::CONJUNCTION>::fromExpression(expr)->right.get(), result);
      break;
    case ExpressionType::DISJUNCTION:
      result.append("| ");
      RegularToPrefixNotation(Regular::BinaryOperation<ExpressionType::DISJUNCTION>::fromExpression(expr)->left.get(), result);
      result.append(" ");
      RegularToPrefixNotation(Regular::BinaryOperation<ExpressionType::DISJUNCTION>::fromExpression(expr)->right.get(), result);
      break;
    case ExpressionType::IMPLICATION:
      result.append("-> ");
      RegularToPrefixNotation(Regular::BinaryOperation<ExpressionType::IMPLICATION>::fromExpression(expr)->left.get(), result);
      result.append(" ");
      RegularToPrefixNotation(Regular::BinaryOperation<ExpressionType::IMPLICATION>::fromExpression(expr)->right.get(), result);
      break;
  }
}

std::string RegularToPrefixNotation(const Regular::Expression* expr) {
  std::string result;
  RegularToPrefixNotation(expr, result);
  return result;
}

std::unique_ptr<Semantic::Expression> RegularToSemantic(const Regular::Expression* expr, std::string_view remains) {
  std::unique_ptr<Semantic::Expression> resultingExpression;
  switch (expr->GetType()) {
    case ExpressionType::BOTTOM:
      assert(remains.find("_|_") == 0);
      resultingExpression = std::make_unique<Semantic::Bottom>(remains.substr(0, 3));
      break;
    case ExpressionType::VARIABLE:
      resultingExpression = std::make_unique<Semantic::Variable>(remains.substr(0, Regular::Variable::fromExpression(expr)->name.size()));
      break;
    case ExpressionType::CONJUNCTION: {
      assert(remains.find("& ") == 0);
      auto lhs = RegularToSemantic(Regular::Conjunction::fromExpression(expr)->left.get(), remains.substr(2));
      assert(remains[2 + lhs->Len()] == ' ');
      auto rhs = RegularToSemantic(Regular::Conjunction::fromExpression(expr)->right.get(), remains.substr(3 + lhs->Len()));
      resultingExpression = std::make_unique<Semantic::Conjunction>(remains.substr(0, 3 + lhs->Len() + rhs->Len()), std::move(lhs), std::move(rhs));
      break;
    }
    case ExpressionType::DISJUNCTION: {
      assert(remains.find("| ") == 0);
      auto lhs = RegularToSemantic(Regular::Disjunction::fromExpression(expr)->left.get(), remains.substr(2));
      assert(remains[2 + lhs->Len()] == ' ');
      auto rhs = RegularToSemantic(Regular::Disjunction::fromExpression(expr)->right.get(), remains.substr(3 + lhs->Len()));
      resultingExpression = std::make_unique<Semantic::Disjunction>(remains.substr(0, 3 + lhs->Len() + rhs->Len()), std::move(lhs), std::move(rhs));
      break;
    }
    case ExpressionType::IMPLICATION:
      assert(remains.find("-> ") == 0);
      auto lhs = RegularToSemantic(Regular::Implication::fromExpression(expr)->left.get(), remains.substr(3));
      assert(remains[3 + lhs->Len()] == ' ');
      auto rhs = RegularToSemantic(Regular::Implication::fromExpression(expr)->right.get(), remains.substr(4 + lhs->Len()));
      resultingExpression = std::make_unique<Semantic::Implication>(remains.substr(0, 4 + lhs->Len() + rhs->Len()), std::move(lhs), std::move(rhs));
      break;
  }
  std::cout << "Result for '" << remains << "' is '" << resultingExpression->GetView() << "'" << std::endl;
  return resultingExpression;
}

} // namespace

namespace Regular {

inline bool operator==(const Expression& lhs, const Expression& rhs) {
  if (lhs.GetType() != rhs.GetType()) {
    return false;
  }
  switch (lhs.GetType()) {
    case ExpressionType::BOTTOM: {
      return true;
      break;
    }
    case ExpressionType::VARIABLE: {
      auto& varLhs = static_cast<const Variable&>(lhs);
      auto& varRhs = static_cast<const Variable&>(rhs);
      return varLhs.name == varRhs.name;
      break;
    }
    case ExpressionType::CONJUNCTION:
    case ExpressionType::DISJUNCTION:
    case ExpressionType::IMPLICATION: {
      auto& bopLhs = static_cast<const BinaryOperationBase&>(lhs);
      auto& bopRhs = static_cast<const BinaryOperationBase&>(rhs);
      return bopLhs.left == bopRhs.left && bopLhs.right == bopRhs.right;
      break;
    }
    default:
      assert(false);
  }
}

std::string ToString(const Expression* expr) {
  return RegularToPrefixNotation(expr);
}

} // namespace Regular

namespace Semantic {

OwningExpression::OwningExpression(const Regular::Expression* expr) : expressionString{RegularToPrefixNotation(expr)} {
  // It is important that `expressionString` is not modified and that it will
  // outlive every node in AST
  root = RegularToSemantic(expr, expressionString);
}

bool operator==(const Semantic::Expression& lhs, const Semantic::Expression& rhs) {
  return lhs.expressionView == rhs.expressionView;
}

} // nnamespace Semantic
