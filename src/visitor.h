class FunctionDeclarationNode;
class BinaryExprNode;
class NumberLiteralNode;
class IdentifierExprNode;
class FunctionCallExprNode;
class BodyNode;
class ConditionalNode;
class DefinitionNode;
class ReturnNode;
class FunctionNode;
class Program;

class Visitor {
public:
  virtual void
  visitFunctionDeclarationNode(const FunctionDeclarationNode *node) = 0;
  virtual void visitBinaryExprNode(const BinaryExprNode *node) = 0;
  virtual void visitNumberLiteralNode(const NumberLiteralNode *node) = 0;
  virtual void visitIdentifierExprNode(const IdentifierExprNode *node) = 0;
  virtual void visitFunctionCallExprNode(const FunctionCallExprNode *node) = 0;
  virtual void visitBodyNode(const BodyNode *node) = 0;
  virtual void visitConditionalNode(const ConditionalNode *node) = 0;
  virtual void visitDefinitionNode(const DefinitionNode *node) = 0;
  virtual void visitReturnNode(const ReturnNode *node) = 0;
  virtual void visitFunctionNode(const FunctionNode *node) = 0;
  virtual void visitProgramNode(const Program *node) = 0;
};