#include "scanner.h"
#include <vector>

class FunctionDeclarationNode {
public:
  FunctionDeclarationNode(const std::string &name,
                          std::vector<std::string> args)
      : name_(name), args_(std::move(args)) {}
  const std::string &getName() { return name_; }
  std::vector<std::string> &getArgs() { return args_; }

private:
  std::string name_;
  std::vector<std::string> args_;
};

class ExprNode {
public:
  enum ExprNodeType {
    BinaryExprNode,
    NumberLiteralNode,
    IdentifierExprNode,
    FunctionCallExprNode,
  };
  ExprNode(ExprNodeType node_type) : node_type_(node_type) {}
  ExprNodeType getExprNodeType() { return node_type_; }

protected:
  ExprNodeType node_type_;
};

class BinaryExprNode : public ExprNode {
public:
  BinaryExprNode(TokenType op, std::unique_ptr<ExprNode> LHS,
                 std::unique_ptr<ExprNode> RHS)
      : operator_(op), lhs_(std::move(LHS)), rhs_(std::move(RHS)),
        ExprNode(ExprNodeType::BinaryExprNode) {}

  ExprNode *getLHS() { return lhs_.get(); }
  ExprNode *getRHS() { return rhs_.get(); }
  TokenType getOperator() { return operator_; }

private:
  std::unique_ptr<ExprNode> lhs_;
  TokenType operator_;
  std::unique_ptr<ExprNode> rhs_;
};

class NumberLiteralNode : public ExprNode {
public:
  NumberLiteralNode(double value)
      : value_(value), ExprNode(ExprNodeType::NumberLiteralNode) {}
  double getValue() { return value_; }

private:
  double value_;
};

class IdentifierExprNode : public ExprNode {
public:
  IdentifierExprNode(const std::string &name)
      : name_(name), ExprNode(ExprNodeType::IdentifierExprNode) {}
  const std::string &getName() { return name_; }

private:
  std::string name_;
};

class FunctionCallExprNode : public ExprNode {
public:
  FunctionCallExprNode(const std::string &name,
                       std::vector<std::unique_ptr<ExprNode>> args)
      : name_(name), args_(std::move(args)),
        ExprNode(ExprNodeType::FunctionCallExprNode) {}
  const std::string &getName() { return name_; }

  std::vector<std::unique_ptr<ExprNode>> &getArgs() { return args_; }

private:
  std::string name_;
  std::vector<std::unique_ptr<ExprNode>> args_;
};

class BodySubNode {
public:
  enum BodyNodeType {
    ConditionalNode,
    ReturnStatementNode,
    DefinitionNode,
  };
  BodySubNode(BodyNodeType node_type) : node_type_(node_type) {}
  BodyNodeType getBodyNodeType() { return node_type_; }

protected:
  BodyNodeType node_type_;
};

class BodyNode {
public:
  BodyNode(std::vector<std::unique_ptr<BodySubNode>> blocks)
      : blocks_(std::move(blocks)) {}
  std::vector<std::unique_ptr<BodySubNode>> &getBlocks() { return blocks_; }

private:
  std::vector<std::unique_ptr<BodySubNode>> blocks_;
};

class ConditionalNode : public BodySubNode {
public:
  ConditionalNode(std::unique_ptr<ExprNode> if_expr,
                  std::unique_ptr<BodyNode> if_body,
                  std::unique_ptr<BodyNode> else_body = nullptr)
      : if_expr_(std::move(if_expr)), if_body_(std::move(if_body)),
        else_body_(std::move(else_body)),
        BodySubNode(BodyNodeType::ConditionalNode) {}
  ExprNode *getIfExpr() { return if_expr_.get(); }
  BodyNode *getIfBody() { return if_body_.get(); }
  BodyNode *getElseBody() { return else_body_.get(); }

private:
  std::unique_ptr<ExprNode> if_expr_;
  std::unique_ptr<BodyNode> if_body_;
  std::unique_ptr<BodyNode> else_body_;
};

class DefinitionNode : public BodySubNode {
public:
  DefinitionNode(std::string lvalue, std::unique_ptr<ExprNode> rhs)
      : lvalue_(lvalue), rhs_(std::move(rhs)),
        BodySubNode(BodyNodeType::DefinitionNode) {}
  const std::string &getLValue() { return lvalue_; }
  ExprNode *getRHS() { return rhs_.get(); }

private:
  std::string lvalue_;
  std::unique_ptr<ExprNode> rhs_;
};

class ReturnNode : public BodySubNode {
public:
  ReturnNode(std::unique_ptr<ExprNode> expr)
      : expr_(std::move(expr)), BodySubNode(BodyNodeType::ReturnStatementNode) {
  }
  ExprNode *getExpr() { return expr_.get(); }

private:
  std::unique_ptr<ExprNode> expr_;
};

class FunctionNode {
public:
  FunctionNode(std::unique_ptr<FunctionDeclarationNode> declaration,
               std::unique_ptr<BodyNode> body)
      : declaration_(std::move(declaration)), body_(std::move(body)) {}
  BodyNode *getBody() { return body_.get(); }
  FunctionDeclarationNode *getFunctionDeclaration() {
    return declaration_.get();
  }

private:
  std::unique_ptr<FunctionDeclarationNode> declaration_;
  std::unique_ptr<BodyNode> body_;
};

class Program {
public:
  Program(std::vector<std::unique_ptr<FunctionNode>> functions)
      : functions_(std::move(functions)) {}
  std::vector<std::unique_ptr<FunctionNode>> &getFunctions() {
    return functions_;
  }

private:
  std::vector<std::unique_ptr<FunctionNode>> functions_;
};

class Parser {
public:
  Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}
  std::unique_ptr<Program> parse();

private:
  std::optional<Token> getCurrentToken();
  std::optional<Token> getNextToken();
  Token expectedNextToken(TokenType type);
  void advance();

  std::unique_ptr<FunctionNode> handleFunction();

  std::unique_ptr<BodyNode> handleBody();

  std::unique_ptr<ConditionalNode> handleConditional();
  std::unique_ptr<ReturnNode> handleReturnStatement();
  std::unique_ptr<DefinitionNode> handleDefinition();

  std::unique_ptr<ExprNode> handleExpression();
  std::unique_ptr<ExprNode> handlePrimary();
  std::unique_ptr<ExprNode> handleBinOpRHS(int8_t precedence,
                                           std::unique_ptr<ExprNode> LHS);
  int32_t getBinOpPrecedence(Token bin_op);

  size_t token_idx_ = 0;
  std::vector<Token> tokens_;
};