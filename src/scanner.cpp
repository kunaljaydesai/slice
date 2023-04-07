#include "scanner.h"
#include <iostream>
#include <optional>
#include <stdexcept>

char Scanner::getChar() const { return source_.at(current_idx_); }

bool Scanner::isAtEnd() const { return current_idx_ >= source_.length(); }

bool Scanner::isSpace() const { return getChar() == ' '; }

bool Scanner::isNewLine() const { return getChar() == '\n'; }

std::optional<Token> Scanner::getToken() {
  skipSpaces();

  if (isAtEnd()) {
    return std::nullopt;
  }

  if (isalpha(getChar())) {
    std::string identifier_string;
    identifier_string += getChar();
    current_idx_ += 1;
    while (!isAtEnd() && isalnum(getChar())) {
      identifier_string += getChar();
      current_idx_ += 1;
    }

    if (identifier_string == "def") {
      return Token(TokenType::tok_def);
    } else if (identifier_string == "extern") {
      return Token(TokenType::tok_extern);
    } else if (identifier_string == "if") {
      return Token(TokenType::tok_if);
    } else if (identifier_string == "else") {
      return Token(TokenType::tok_else);
    }
    return Token(TokenType::tok_identifier, identifier_string);
  } else if (isdigit(getChar())) {
    std::string number_string = std::to_string(getChar());
    current_idx_ += 1;
    while (!isAtEnd() && isdigit(getChar())) {
      number_string += std::to_string(getChar());
      current_idx_ += 1;
    }

    if (getChar() == '.') {
      number_string += std::to_string(getChar());
      current_idx_ += 1;
      while (!isAtEnd() && isdigit(getChar())) {
        number_string += std::to_string(getChar());
        current_idx_ += 1;
      }
    }

    return Token(TokenType::tok_number, std::stod(number_string));
  } else if (getChar() == '#') {
    // TODO(kunal): make a helper method to skip line
    current_idx_ += 1;
    while (!isAtEnd() && !isNewLine()) {
      current_idx_ += 1;
    }
    return getToken();
  } else if (getChar() == '(') {
    current_idx_ += 1;
    return Token(TokenType::tok_lpar);
  } else if (getChar() == ')') {
    current_idx_ += 1;
    return Token(TokenType::tok_lpar);
  } else if (getChar() == '{') {
    current_idx_ += 1;
    return Token(TokenType::tok_lbrak);
  } else if (getChar() == '}') {
    current_idx_ += 1;
    return Token(TokenType::tok_rbrak);
  } else if (getChar() == '<') {
    current_idx_ += 1;
    if (!isAtEnd() && getChar() == '=') {
      current_idx_ += 1;
      return Token(TokenType::tok_lte);
    }
    return Token(TokenType::tok_lt);
  } else if (getChar() == '>') {
    current_idx_ += 1;
    if (!isAtEnd() && getChar() == '=') {
      current_idx_ += 1;
      return Token(TokenType::tok_gte);
    }
    return Token(TokenType::tok_gt);
  } else if (getChar() == '+') {
    current_idx_ += 1;
    return Token(TokenType::tok_add);
  } else if (getChar() == '-') {
    current_idx_ += 1;
    return Token(TokenType::tok_sub);
  } else if (getChar() == '*') {
    current_idx_ += 1;
    return Token(TokenType::tok_mul);
  } else if (getChar() == '/') {
    current_idx_ += 1;
    return Token(TokenType::tok_div);
  } else if (getChar() == '%') {
    current_idx_ += 1;
    return Token(TokenType::tok_div);
  }
  throw std::runtime_error("Unidentified char");
}

void Scanner::skipSpaces() {
  while (!isAtEnd() && isspace(source_.at(current_idx_))) {
    current_idx_ += 1;
  }
}

void Scanner::scanTokens() {
  while (!isAtEnd()) {
    auto token = getToken();
    if (token) {
      tokens_.push_back(*token);
    }
  }
  tokens_.push_back(Token(TokenType::tok_eof));
}
