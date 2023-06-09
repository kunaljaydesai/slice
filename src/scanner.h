#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <vector>

enum TokenType {
  tok_eof,
  // keywords
  tok_def,
  tok_extern,
  tok_if,
  tok_else,
  tok_return,

  // variables
  tok_identifier,

  // literals
  tok_number,

  // misc
  tok_rpar,
  tok_lpar,
  tok_rbrak,
  tok_lbrak,
  tok_comma,

  // assignment
  tok_equals,

  // comparators
  tok_lt,
  tok_gt,
  tok_lte,
  tok_gte,

  // operations
  tok_add,
  tok_sub,
  tok_mul,
  tok_div,
  tok_mod,
};

class Token {

public:
  Token(TokenType type) : type_(type) {}
  Token(TokenType type, std::string identifier)
      : type_(type), identifier_(identifier) {}
  Token(TokenType type, double number) : type_(type), number_(number) {}

  bool operator!=(const Token &other) const { return !this->equals(other); }
  bool operator==(const Token &other) const { return this->equals(other); }

  const bool equals(const Token &other) const {
    if (other.getType() != getType()) {
      return false;
    }

    TokenType type = getType();
    if (getType() == tok_identifier) {
      return other.getIdentifier() == getIdentifier();
    } else if (getType() == tok_number) {
      return other.getNumber() == getNumber();
    }
    return true;
  }
  const TokenType getType() const { return type_; }
  const std::string getIdentifier() const { return identifier_; }
  const double getNumber() const { return number_; }
  const std::string toString() const {
    if (getType() == tok_identifier) {
      return getIdentifier();
    } else if (getType() == tok_number) {
      return std::to_string(getNumber());
    }
    std::string type = "";
    switch (getType()) {
    case tok_lt:
      type = "less than";
      break;
    case tok_gt:
      type = "greater than";
      break;
    case tok_lte:
      type = "less than equal";
      break;
    case tok_lbrak:
      type = "left bracket";
      break;
    case tok_lpar:
      type = "left paren";
      break;
    case tok_if:
      type = "if";
      break;
    default:
      break;
    }
    if (type.size() == 0) {
      return "Token: " + std::to_string(getType());
    }
    return "Token: " + type;
  }

private:
  const TokenType type_;
  const std::string identifier_ = ""; // filled in if type tok_identifier
  const double number_ = 0;           // filled in if type tok_number
};

class Scanner {
public:
  Scanner(std::string source) : source_(source) {}
  void scanTokens();
  const std::vector<Token> &tokens() { return tokens_; }

private:
  void skipSpaces();
  std::optional<Token> getToken();

  char getChar() const;

  bool isSpace() const;
  bool isAtEnd() const;
  bool isNewLine() const;

  const std::string source_;
  uint32_t current_idx_ = 0;
  std::vector<Token> tokens_;
};