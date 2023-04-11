#include "../src/parser.h"

#include <assert.h>
#include <iostream>
#include <string>

const std::string basic = "# Compute the x'th fibonacci number.\n"
                          "def fib(x){\n"
                          "if (x < 3) {\n"
                          "return 1\n"
                          "} else {\n"
                          "return fib(x-1)+fib(x-2)\n"
                          "}\n"
                          "}\n"
                          "# This expression will compute the 40th number.\n";

void runBasicTest() {
  Scanner scanner(basic);
  scanner.scanTokens();
  const std::vector<Token> expected = {
      Token(TokenType::tok_def),
      Token(TokenType::tok_identifier, "fib"),
      Token(TokenType::tok_lpar),
      Token(TokenType::tok_identifier, "x"),
      Token(TokenType::tok_rpar),
      Token(TokenType::tok_lbrak),
      Token(TokenType::tok_if),
      Token(TokenType::tok_lpar),
      Token(TokenType::tok_identifier, "x"),
      Token(TokenType::tok_lt),
      Token(TokenType::tok_number, 3),
      Token(TokenType::tok_rpar),
      Token(TokenType::tok_lbrak),
      Token(TokenType::tok_return),
      Token(TokenType::tok_number, 1),
      Token(TokenType::tok_rbrak),
      Token(TokenType::tok_else),
      Token(TokenType::tok_lbrak),
      Token(TokenType::tok_return),
      Token(TokenType::tok_identifier, "fib"),
      Token(TokenType::tok_lpar),
      Token(TokenType::tok_identifier, "x"),
      Token(TokenType::tok_sub),
      Token(TokenType::tok_number, 1),
      Token(TokenType::tok_rpar),
      Token(TokenType::tok_add),
      Token(TokenType::tok_identifier, "fib"),
      Token(TokenType::tok_lpar),
      Token(TokenType::tok_identifier, "x"),
      Token(TokenType::tok_sub),
      Token(TokenType::tok_number, 2),
      Token(TokenType::tok_rpar),
      Token(TokenType::tok_rbrak),
      Token(TokenType::tok_rbrak),
  };
  std::vector<Token> actual = scanner.tokens();
  for (size_t i = 0; i < expected.size(); i++) {
    Token actual_token = actual[i];
    Token expected_token = expected[i];
    if (actual_token != expected_token) {
      std::cout << "Expected " << expected[i].toString() << ", got "
                << scanner.tokens()[i].toString() << std::endl;
      throw std::runtime_error("Test failed");
    }
  }
  Parser parser = Parser(actual);
  std::unique_ptr<Program> program = parser.parse();
  const auto &functions = program->getFunctions();
  assert(functions.size() == 1);
  const auto &function = program->getFunctions()[0];
  const auto &func_declaration = function->getFunctionDeclaration();
  assert(func_declaration->getName() == "fib");
  assert(func_declaration->getArgs().size() == 1);
  assert(func_declaration->getArgs()[0] == "x");
  const auto &func_body = function->getBody();
  const auto &blocks = func_body->getBlocks();
  assert(blocks.size() == 1);
  assert(blocks[0].get()->getBodyNodeType() ==
         BodySubNode::BodyNodeType::ConditionalNode);
  const auto &if_block = static_cast<ConditionalNode *>(blocks[0].get());
  assert(if_block->getIfExpr()->getExprNodeType() == ExprNode::BinaryExprNode);
  const auto &if_expr = static_cast<BinaryExprNode *>(if_block->getIfExpr());
  assert(if_expr->getOperator() == tok_lt);
  const auto &if_expr_lhs =
      static_cast<IdentifierExprNode *>(if_expr->getLHS());
  assert(if_expr_lhs->getName() == "x");
  const auto &if_expr_rhs = static_cast<NumberLiteralNode *>(if_expr->getRHS());
  assert(if_expr_rhs->getValue() == 3);

  const auto &if_body = if_block->getIfBody();
  const auto &if_body_blocks = if_body->getBlocks();
  assert(if_body_blocks.size() == 1);
  assert(if_body_blocks[0]->getBodyNodeType() ==
         BodySubNode::BodyNodeType::ReturnStatementNode);
  const auto &return_block = static_cast<ReturnNode *>(if_body_blocks[0].get());
  assert(return_block->getExpr()->getExprNodeType() ==
         ExprNode::ExprNodeType::NumberLiteralNode);
  const auto &return_block_expr =
      static_cast<NumberLiteralNode *>(return_block->getExpr());
  assert(return_block_expr->getValue() == 1);

  const auto &else_body = if_block->getElseBody();
  const auto &else_body_blocks = else_body->getBlocks();
  assert(else_body_blocks.size() == 1);
  assert(else_body_blocks[0]->getBodyNodeType() ==
         BodySubNode::BodyNodeType::ReturnStatementNode);
  const auto &else_return_block =
      static_cast<ReturnNode *>(else_body_blocks[0].get());
  assert(else_return_block->getExpr()->getExprNodeType() ==
         ExprNode::BinaryExprNode);
  const auto &else_return_expr =
      static_cast<BinaryExprNode *>(else_return_block->getExpr());
  assert(else_return_expr->getOperator() == tok_add);
  const auto &else_return_lhs =
      static_cast<FunctionCallExprNode *>(else_return_expr->getLHS());
  assert(else_return_lhs->getExprNodeType() == ExprNode::FunctionCallExprNode);
  assert(else_return_lhs->getName() == "fib");
  assert(else_return_lhs->getArgs().size() == 1);
  const auto &first_arg =
      static_cast<BinaryExprNode *>(else_return_lhs->getArgs()[0].get());
  assert(first_arg->getExprNodeType() == ExprNode::BinaryExprNode);
  assert(first_arg->getOperator() == tok_sub);
  assert(first_arg->getLHS()->getExprNodeType() ==
         ExprNode::IdentifierExprNode);

  assert(static_cast<IdentifierExprNode *>(first_arg->getLHS())->getName() ==
         "x");
  assert(first_arg->getRHS()->getExprNodeType() == ExprNode::NumberLiteralNode);
  assert(static_cast<NumberLiteralNode *>(first_arg->getRHS())->getValue() ==
         1);

  const auto &else_return_rhs =
      static_cast<FunctionCallExprNode *>(else_return_expr->getRHS());
  assert(else_return_rhs->getExprNodeType() == ExprNode::FunctionCallExprNode);
  assert(else_return_rhs->getName() == "fib");
  assert(else_return_rhs->getArgs().size() == 1);
  const auto &first_arg_rhs =
      static_cast<BinaryExprNode *>(else_return_rhs->getArgs()[0].get());
  assert(first_arg_rhs->getExprNodeType() == ExprNode::BinaryExprNode);
  assert(first_arg_rhs->getOperator() == tok_sub);
  assert(first_arg_rhs->getLHS()->getExprNodeType() ==
         ExprNode::IdentifierExprNode);
  assert(
      static_cast<IdentifierExprNode *>(first_arg_rhs->getLHS())->getName() ==
      "x");
  assert(first_arg_rhs->getRHS()->getExprNodeType() ==
         ExprNode::NumberLiteralNode);
  assert(
      static_cast<NumberLiteralNode *>(first_arg_rhs->getRHS())->getValue() ==
      2);
}

int main(int argc, char **argv) {
  runBasicTest();
  std::cout << "Tests succeeded!" << std::endl;
  return 0;
}