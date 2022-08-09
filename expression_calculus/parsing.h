#pragma once

#include "expression.h"

#include <string>
#include <vector>
#include <string_view>
#include <optional>
#include <cassert>
#include <algorithm>
#include <memory>
#include <stdexcept>

/*******************************************************************************
*                                  Tokenizer                                  *
*******************************************************************************/

enum class TokenType {
  TURNSTILE,
  ARROW,
  AMPERSAND,
  BAR,
  EXCLAMATION,
  LPAREN,
  RPAREN,
  COMMA,
  VARIABLE
};

// TODO: cover with tests, this is an easily testable part in which it's too
// easy to write a bug
struct Tokenizer {
public:
  using Token = std::pair<TokenType, std::string_view>;

  Tokenizer(std::string line) : currentToken{0}, tokenizedString{std::move(line)} {
    // tokenization process (same for A and B)
    using namespace std::literals;  // for ""sv
    std::string_view v{tokenizedString.data(), tokenizedString.size()};
    const auto tryMatchPrefix = [] (std::string_view remains) -> std::optional<Token> {
      static constexpr std::initializer_list<Token> simpleTokens = {
        {TokenType::TURNSTILE, "|-"sv},
        {TokenType::ARROW, "->"sv},
        {TokenType::AMPERSAND, "&"sv},
        {TokenType::BAR, "|"sv},
        {TokenType::EXCLAMATION, "!"sv},
        {TokenType::LPAREN, "("sv},
        {TokenType::RPAREN, ")"sv},
        {TokenType::COMMA, ","sv},
      };
      for (const auto& [tokenType, tok]: simpleTokens) {
        if (remains.substr(0, tok.size()) == tok) {
          remains.remove_prefix(tok.size());
          return std::make_pair(tokenType, tok);
        }
      }
      return std::nullopt;
    };
    while (true) {
      // trim whitespace characters from the beginning (if they are present)
      auto pos = v.find_first_not_of(" \t\r\f\v");
      if (pos != std::string_view::npos) {
        v.remove_prefix(pos);
      }
      if (v.empty()) {
        break;
      }
      // now try to find one of tokens (varible, |-, ->, |, &, !, left-paren,
      // right-paren, comma)
      // TODO: Improve the design (lambda mutates the view)
      if (auto matchedToken = tryMatchPrefix(v)) {
        auto [tokenType, tok] = *matchedToken;
        v.remove_prefix(tok.size());
        tokens.emplace_back(tokenType, tok);
      } else {
        // we have a variable - it is 100% of needed format but we better verify
        // it with asserts
        assert('A' <= v[0] && v[0] <= 'Z');
        auto new_start = std::find_if_not(v.begin(), v.end(), [] (char c) {
            return ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '\'';
        });
        tokens.emplace_back(TokenType::VARIABLE, v.substr(0, new_start - v.begin()));
        v.remove_prefix(new_start - v.begin());
      }
    }
  }

  Tokenizer& operator=(const Tokenizer&) = delete;
  Tokenizer& operator=(Tokenizer&&) = delete;

  std::optional<Token> NextToken() {
    auto result = Peek();
    currentToken++;
    return result;
  }

  std::optional<Token> Peek() {
    if (currentToken >= tokens.size()) {
      return std::nullopt;
    }
    return tokens[currentToken];
  }

  void Reset() {
    currentToken = 0;
  }

  const std::vector<Token>& GetTokens() const {
    return tokens;
  }

private:
  std::size_t currentToken;
  std::vector<std::pair<TokenType, std::string_view>> tokens;
  std::string tokenizedString;
};

/*******************************************************************************
*                                   Parser                                    *
*******************************************************************************/

struct Parser {
public:
  // TODO: test via this constructor, it should be pretty easy at this point
  // (although for matching purposes we may want to use prefix notation)
  Parser(std::string expressionLine) : tokenizer{std::make_unique<Tokenizer>(std::move(expressionLine))} {}
  Parser(std::unique_ptr<Tokenizer>&& t) : tokenizer{std::move(t)} {}

  std::unique_ptr<Regular::Expression> Parse() {
    std::unique_ptr<Regular::Expression> result;

    tokenizer->Reset();  // so that we can parse the expression again :)
    return result;
  }

  std::unique_ptr<Regular::Expression> ParsePrim() {
    if (!tokenizer->Peek()) {
      throw std::runtime_error{"Token expected at the beginning of expression primitive"};
    }
    auto [tokenType, token] = *tokenizer->NextToken();
    switch (tokenType) {
      case TokenType::LPAREN: {
        std::unique_ptr<Regular::Expression> expr = ParseExpression();
        if (auto rparen = tokenizer->NextToken(); !rparen || rparen->first != TokenType::RPAREN) {
          throw std::runtime_error{std::string{"Closing parenthesis expected, got "} + (rparen ? std::string{rparen->second} : "no tokens")};
        }
        return expr;
      }
      case TokenType::VARIABLE: {
        return std::make_unique<Regular::Variable>(std::string{token});
      }
      case TokenType::EXCLAMATION: {
        return std::make_unique<Regular::Implication>(ParsePrim(), std::make_unique<Regular::Bottom>());
      }
      default:
        throw std::runtime_error{"Unexpected token at the start of primary expression"};
    }
  }

  std::unique_ptr<Regular::Expression> ParseConjunctionSequence() {
    std::unique_ptr<Regular::Expression> result = ParsePrim();
    while (tokenizer->Peek() && tokenizer->Peek()->first == TokenType::AMPERSAND) {
      tokenizer->NextToken();
      result = std::make_unique<Regular::Conjunction>(std::move(result), ParsePrim());
    }
    return result;
  }

  std::unique_ptr<Regular::Expression> ParseDisjunctionSequence() {
    std::unique_ptr<Regular::Expression> result = ParseConjunctionSequence();
    while (tokenizer->Peek() && tokenizer->Peek()->first == TokenType::BAR) {
      tokenizer->NextToken();
      result = std::make_unique<Regular::Disjunction>(std::move(result), ParsePrim());
    }
    return result;
  }

  std::unique_ptr<Regular::Expression> ParseExpression() {
    std::vector<std::unique_ptr<Regular::Expression>> clauses;
    clauses.emplace_back(ParseDisjunctionSequence());
    while (tokenizer->Peek() && tokenizer->Peek()->first == TokenType::ARROW) {
      tokenizer->NextToken();
      clauses.emplace_back(ParseDisjunctionSequence());
    }
    std::unique_ptr<Regular::Expression> result = std::move(clauses.back());
    clauses.pop_back();
    std::reverse(clauses.begin(), clauses.end());
    // I decided not to use std::accumulate (accumulate, not reduce because the
    // order is important!) because the code becomes less readable and less
    // predictable
    for (auto& clause : clauses) {
      result = std::make_unique<Regular::Implication>(std::move(clause), std::move(result));
    }
    return result;
  }

private:
  std::unique_ptr<Tokenizer> tokenizer;
};
