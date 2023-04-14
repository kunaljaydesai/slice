#include "visitor.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>
#include <iostream>

class CodegenVisitor : public Visitor {
public:
  CodegenVisitor() {
    context_ = std::make_unique<llvm::LLVMContext>();
    module_ = std::make_unique<llvm::Module>("slice", *context_);
    builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);
  }
  void visitBinaryExprNode(const BinaryExprNode *node) override;
  void visitNumberLiteralNode(const NumberLiteralNode *node) override;
  void visitIdentifierExprNode(const IdentifierExprNode *node) override;
  void visitFunctionCallExprNode(const FunctionCallExprNode *node) override;
  void visitBodyNode(const BodyNode *node) override;
  void visitConditionalNode(const ConditionalNode *node) override;
  void visitDefinitionNode(const DefinitionNode *node) override;
  void visitReturnNode(const ReturnNode *node) override;
  void
  visitFunctionDeclarationNode(const FunctionDeclarationNode *node) override;
  void visitFunctionNode(const FunctionNode *node) override;
  void visitProgramNode(const Program *node) override;

  void dump() { module_->print(llvm::outs(), nullptr); }

private:
  std::unique_ptr<llvm::LLVMContext> context_;
  std::unique_ptr<llvm::Module> module_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;
  llvm::Value *ret_;
};