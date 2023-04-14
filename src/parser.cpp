#include "parser.h"
#include "visitor.h"

#include <iostream>

std::optional<Token> Parser::getCurrentToken() {
  if (token_idx_ >= tokens_.size()) {
    return std::nullopt;
  }
  return tokens_[token_idx_];
}

std::optional<Token> Parser::getNextToken() {
  advance();
  return getCurrentToken();
}

Token Parser::expectedNextToken(TokenType type) {
  auto token = getNextToken();
  if (!token) {
    std::cout << "Expecting next token " << type << " but didn't find one"
              << std::endl;
    exit(1);
  }

  if (token->getType() != type) {
    std::cout << "Expected token type " << type << " at " << token_idx_
              << ", but got " << token->getType() << std::endl;
    exit(1);
  }

  return *token;
}

void Parser::advance() { token_idx_++; }

std::unique_ptr<ExprNode> Parser::handlePrimary() {
  auto token = getCurrentToken();
  if (!token) {
    std::cout << "Expected token in expression";
    exit(1);
  }

  switch (token->getType()) {
  case tok_identifier: {
    // peek ahead to see if its a fn call or a variable
    bool is_fn_call = token_idx_ + 1 < tokens_.size() &&
                      tokens_[token_idx_ + 1].getType() == tok_lpar;
    if (is_fn_call) {
      std::vector<std::unique_ptr<ExprNode>> args;
      expectedNextToken(tok_lpar);
      while (auto token = getNextToken()) {
        if (token->getType() != tok_identifier) {
          break;
        }
        args.push_back(handleExpression());
        if (!getCurrentToken() || getCurrentToken()->getType() != tok_comma) {
          break;
        }
        expectedNextToken(tok_comma);
      }
      advance(); // skip past rpar
      return std::make_unique<FunctionCallExprNode>(token->getIdentifier(),
                                                    std::move(args));
    }
    advance();
    return std::make_unique<IdentifierExprNode>(token->getIdentifier());
  }
  case tok_number:
    advance();
    return std::make_unique<NumberLiteralNode>(token->getNumber());
  case tok_lpar: {
    advance();
    auto expr = handleExpression();
    if (!getCurrentToken() || getCurrentToken()->getType() != tok_rpar) {
      std::cout << "error when handling parenthesized expression, got token "
                << getCurrentToken()->getType()
                << " expected tok_rpar, at token idx " << token_idx_
                << std::endl;
      exit(1);
    }
    advance();
    return expr;
  }
  default:
    std::cout << "unexpected token when parsing primary expr" << std::endl;
    exit(1);
  }
}

int32_t Parser::getBinOpPrecedence(Token bin_op) {
  switch (bin_op.getType()) {
  case tok_add:
    return 0;
  case tok_sub:
    return 1000;
  case tok_mul:
    return 2000;
  case tok_div:
    return 3000;
  case tok_mod:
    return 4000;
  case tok_gt:
    return 5000;
  case tok_gte:
    return 6000;
  case tok_lt:
    return 7000;
  case tok_lte:
    return 8000;
  default:
    return -1;
  }
}
std::unique_ptr<ExprNode>
Parser::handleBinOpRHS(int8_t precedence, std::unique_ptr<ExprNode> LHS) {
  while (true) {

    auto bin_op = getCurrentToken();

    if (!bin_op) {
      std::cout << "Expected token in expression";
      exit(1);
    }

    int32_t bin_op_precedence = getBinOpPrecedence(*bin_op);
    if (getBinOpPrecedence(*bin_op) < precedence) {
      return LHS;
    }
    advance();

    auto RHS = handlePrimary();
    if (!RHS) {
      std::cout << "Expected expression after binary operator" << std::endl;
      exit(1);
    }

    auto next_bin_op = getCurrentToken();
    if (!next_bin_op) {
      return std::make_unique<BinaryExprNode>(bin_op->getType(), std::move(LHS),
                                              std::move(RHS));
    }

    int32_t next_bin_op_precedence = getBinOpPrecedence(*next_bin_op);
    if (bin_op_precedence < next_bin_op_precedence) {
      RHS = handleBinOpRHS(next_bin_op_precedence + 1, std::move(RHS));
    }

    LHS = std::make_unique<BinaryExprNode>(bin_op->getType(), std::move(LHS),
                                           std::move(RHS));
  }
}

std::unique_ptr<ExprNode> Parser::handleExpression() {
  auto primary = handlePrimary();
  return handleBinOpRHS(0, std::move(primary));
}

std::unique_ptr<ConditionalNode> Parser::handleConditional() {
  advance(); // skip if token
  auto if_expr = handleExpression();

  advance(); // skip lbrak
  auto if_body = handleBody();
  advance(); // skip rbrak
  if (auto token = getCurrentToken()) {
    if (token->getType() == tok_else) {
      expectedNextToken(tok_lbrak);
      advance(); // skip lbrak
      auto else_body = handleBody();
      advance(); // skip rbrak
      return std::make_unique<ConditionalNode>(
          std::move(if_expr), std::move(if_body), std::move(else_body));
    }
  }

  return std::make_unique<ConditionalNode>(std::move(if_expr),
                                           std::move(if_body));
}

std::unique_ptr<ReturnNode> Parser::handleReturnStatement() {
  //   expectedNextToken(tok_return);
  advance(); // skip return
  return std::make_unique<ReturnNode>(std::move(handleExpression()));
}

std::unique_ptr<DefinitionNode> Parser::handleDefinition() {
  auto lvalue = getCurrentToken();
  expectedNextToken(tok_equals);
  advance();
  auto expr = handleExpression();
  return std::make_unique<DefinitionNode>(lvalue->getIdentifier(),
                                          std::move(expr));
}

std::unique_ptr<BodyNode> Parser::handleBody() {
  std::vector<std::unique_ptr<BodySubNode>> blocks;
  while (auto token = getCurrentToken()) {
    if (!token) {
      std::cout << "Expected another token in function body" << std::endl;
      exit(1);
    }

    switch (token->getType()) {
    case tok_if: {
      blocks.push_back(handleConditional());
      break;
    }
    case tok_return: {
      blocks.push_back(handleReturnStatement());
      break;
    }
    case tok_identifier: {
      blocks.push_back(handleDefinition());
      break;
    }
    case tok_rbrak: {
      return std::make_unique<BodyNode>(std::move(blocks));
    }
    default: {
      std::cout << "Expected conditional, return statemnet, or identifier in "
                   "function body but got "
                << token->toString() << " at " << token_idx_ << std::endl;
      exit(1);
    }
    }
    if (getCurrentToken() && getCurrentToken()->getType() == tok_rbrak) {
      break;
    }
  }
  return std::make_unique<BodyNode>(std::move(blocks));
}

std::unique_ptr<FunctionNode> Parser::handleFunction() {
  // consume function name
  std::optional<Token> fnName = getNextToken();
  if (!fnName || (*fnName).getType() != tok_identifier) {
    std::cout << "Error, expecting identifer in function declaration"
              << std::endl;
  }

  // consume lparenthesis
  expectedNextToken(tok_lpar);

  auto token = getNextToken();
  std::vector<std::string> args;
  while (token && token->getType() != tok_rpar) {
    if (token->getType() == tok_identifier) {
      args.push_back(token->getIdentifier());
      advance();

      if (!getCurrentToken()) {
        std::cout << "Error when parsing function declaration, expected comma"
                  << std::endl;
        exit(1);
      }

      token.emplace(*getCurrentToken());
      if (token->getType() == tok_rpar) {
        break;
      }

      if (token->getType() != tok_comma) {
        std::cout << "Error when parsing function declaration, expected comma"
                  << std::endl;
      }
      advance();

    } else {
      std::cout << "Error when parsing function declaration" << std::endl;
      exit(1);
    }
  }

  auto functionDeclaration = std::make_unique<FunctionDeclarationNode>(
      fnName->getIdentifier(), std::move(args));

  expectedNextToken(tok_lbrak);
  advance(); // skip lbrak
  auto body = handleBody();
  advance(); // skip rbrak
  return std::make_unique<FunctionNode>(std::move(functionDeclaration),
                                        std::move(body));
}

std::unique_ptr<Program> Parser::parse() {
  std::vector<std::unique_ptr<FunctionNode>> functions;
  while (auto token = getCurrentToken()) {
    switch (token->getType()) {
    case tok_eof:
      return std::make_unique<Program>(std::move(functions));
    case tok_def:
      functions.push_back(std::move(handleFunction()));
      break;
    default:
      std::cout << "Error: received invalid token type " << token->toString()
                << std::endl;
      exit(1);
    }
  }
  return std::make_unique<Program>(std::move(functions));
}

void FunctionDeclarationNode::accept(Visitor *v) {
  v->visitFunctionDeclarationNode(this);
}
void BinaryExprNode::accept(Visitor *v) { v->visitBinaryExprNode(this); }
void NumberLiteralNode::accept(Visitor *v) { v->visitNumberLiteralNode(this); }
void IdentifierExprNode::accept(Visitor *v) {
  v->visitIdentifierExprNode(this);
}
void FunctionCallExprNode::accept(Visitor *v) {
  v->visitFunctionCallExprNode(this);
}
void BodyNode::accept(Visitor *v) { v->visitBodyNode(this); }
void ConditionalNode::accept(Visitor *v) { v->visitConditionalNode(this); }
void DefinitionNode::accept(Visitor *v) { v->visitDefinitionNode(this); }
void ReturnNode::accept(Visitor *v) { v->visitReturnNode(this); }
void FunctionNode::accept(Visitor *v) { v->visitFunctionNode(this); }
void Program::accept(Visitor *v) { v->visitProgramNode(this); }